// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "ArchivItem_Bob.h"
#include "ArchivItem_Bitmap_Player.h"
#include "ErrorCodes.h"
#include "IAllocator.h"
#include "libsiedler2.h"
#include "loadMapping.h"
#include "libendian/EndianIStreamAdapter.h"
#include "s25util/StringConversion.h"
#include <boost/range/adaptor/indexed.hpp>
#include <iostream>
#include <limits>

namespace {
constexpr uint16_t COLOR_BLOCK_HEADER = 0x01F5;
constexpr uint16_t IMAGE_DATA_HEADER = 0x01F4;
/// 8 Animation Steps, 6 directions, 2 types (Fat, Non-Fat) --> Array [fat][direction][animStep]
constexpr unsigned NUM_BODY_IMAGES = 2 * 6 * 8;
/// Same for links but Array [animStep][fat][direction]
constexpr unsigned NUM_LINKS_PER_OVERLAY = 8 * 2 * 6;
constexpr uint16_t SPRITE_WIDTH = 32;
/// Draw offset in X
constexpr uint16_t X_OFFSET = 16;
} // namespace

namespace libsiedler2 {
/// Read a block of colors used later
static int readColorBlock(libendian::EndianIStreamAdapter<false, std::istream&>& fs, std::vector<uint8_t>& pixels)
{
    uint16_t id, size;
    if(!(fs >> id >> size))
        return ErrorCode::UNEXPECTED_EOF;

    if(id != COLOR_BLOCK_HEADER)
        return ErrorCode::WRONG_FORMAT;

    pixels.resize(size);
    if(!(fs >> pixels))
        return ErrorCode::UNEXPECTED_EOF;
    return ErrorCode::NONE;
}
/// Read a chunk of image data (array with start indices into a color array and y-offset)
static int readImageData(libendian::EndianIStreamAdapter<false, std::istream&>& fs, std::vector<uint16_t>& starts,
                         uint8_t& ny)
{
    uint16_t id;
    uint8_t height;
    if(!(fs >> id >> height))
        return ErrorCode::UNEXPECTED_EOF;

    if(id != IMAGE_DATA_HEADER)
        return ErrorCode::WRONG_FORMAT;

    starts.resize(height);
    if(!(fs >> starts >> ny))
        return ErrorCode::UNEXPECTED_EOF;
    return ErrorCode::NONE;
}

/**
 *  At which index in this archive the overlay with the given id is stored.
 *  
 *  @param[in] overlayId the id of an overlay, stored in spriteOverlayId
 *  
 *  @return The archive index for the given overlay.
 */
static inline unsigned getOverlayArchiveIdx(unsigned overlayId)
{   
    // The first NUM_BODY_IMAGES indices store the body images. Overlays are shifted accordingly.
    return NUM_BODY_IMAGES + overlayId; 
}

ArchivItem_Bob::ArchivItem_Bob() : ArchivItem(BobType::Bob), numOverlayImgs(0) {}

ArchivItem_Bob::~ArchivItem_Bob() = default;

/**
 *  lädt die Bobdaten aus einer Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Bob::load(std::istream& file, const ArchivItem_Palette* palette)
{
    if(!file)
        return ErrorCode::FILE_NOT_ACCESSIBLE;
    if(!palette)
        return ErrorCode::PALETTE_MISSING;

    libendian::EndianIStreamAdapter<false, std::istream&> fs(file);

    // Read body color block
    std::vector<uint8_t> raw_base;
    if(int ec = readColorBlock(fs, raw_base))
        return ec;

    // Read body images (to get full figure draw this then draw the item image (below) over it)
    // They form an array [fat][direction][animStep]
    alloc(NUM_BODY_IMAGES);
    for(uint32_t i = 0; i < NUM_BODY_IMAGES; ++i)
    {
        std::vector<uint16_t> starts;
        uint8_t ny;
        if(int ec = readImageData(fs, starts, ny))
            return ec;

        auto image = getAllocator().create<ArchivItem_Bitmap_Player>(BobType::BitmapPlayer);
        assert(image);
        image->setNx(X_OFFSET);
        image->setNy(ny);

        int ec = image->load(SPRITE_WIDTH, raw_base, starts, true, palette);
        if(ec)
            return ec;

        set(i, std::move(image));
    }

    // Color blocks for each direction
    std::array<std::vector<uint8_t>, 6> raw;

    for(auto& i : raw)
    {
        if(int ec = readColorBlock(fs, i))
            return ec;
    }

    // Number of overlay images (e.g. goods of carriers)
    fs >> numOverlayImgs;

    alloc_inc(numOverlayImgs);

    std::vector<std::vector<uint16_t>> starts(numOverlayImgs);
    std::vector<uint8_t> ny(numOverlayImgs);

    for(uint16_t i = 0; i < numOverlayImgs; ++i)
    {
        if(int ec = readImageData(fs, starts[i], ny[i]))
            return ec;
    }

    // Number of sprites, i.e. complete pictures of a body and an overlay.
    uint16_t numSprites;
    if(!(fs >> numSprites))
        return ErrorCode::UNEXPECTED_EOF;

    // For each sprite we read an overlay id and an ny-offset.
    // For a complete sprite image, draw the body [fat][direction][animStep] and then draw
    // the overlay at NUM_BODY_IMAGES + [bobId][animStep][fat][direction] at overlay.getNy() + nyOffset
    spriteOverlayIdx.resize(numSprites);
    spriteOverlayNyOffset.resize(numSprites);

    // Read the sprites.
    for(uint32_t i = 0; i < numSprites; ++i)
    {
        // The overlay id that the sprite uses.
        uint16_t overlayImgId;
        // The ny offset for this sprite. Overlays have a standard ny offset, and 
        // animation steps may use slightly different offsets.
        int16_t nyOffset;

        if(!(fs >> overlayImgId >> nyOffset))
            return ErrorCode::UNEXPECTED_EOF;
        if(overlayImgId >= numOverlayImgs)
            return ErrorCode::WRONG_FORMAT;

        const unsigned overlayIdx = getOverlayArchiveIdx(overlayImgId);
        spriteOverlayIdx[i] = overlayIdx;
        spriteOverlayNyOffset[i] = nyOffset;

        // Load each overlay image just once, they are used for multiple sprites.
        if (get(overlayIdx))
            continue;

        auto overlayImg = getAllocator().create<ArchivItem_Bitmap_Player>(BobType::BitmapPlayer);
        assert(overlayImg);
        overlayImg->setNx(X_OFFSET);
        overlayImg->setNy(ny[overlayImgId]);
        if(int ec = overlayImg->load(SPRITE_WIDTH, raw[i % 6], starts[overlayImgId], true, palette))
            return ec;
        set(overlayIdx, std::move(overlayImg));
    }

    return (!fs) ? ErrorCode::UNEXPECTED_EOF : ErrorCode::NONE;
}

/**
 *  schreibt die Bobdaten in eine Datei.
 *
 *  @param[in] file    Dateihandle der Datei
 *  @param[in] palette Grundpalette
 *
 *  @return liefert Null bei Erfolg, ungleich Null bei Fehler
 */
int ArchivItem_Bob::write(std::ostream&, const ArchivItem_Palette*)
{
    return ErrorCode::UNSUPPORTED_FORMAT;
}

ArchivItem_Bitmap_Player* ArchivItem_Bob::getBody(bool fat, ImgDir direction, unsigned animationstep)
{
    // Array: [fat][direction][animStep]: [2][6][8]
    const unsigned bodyIdx = (fat * 6 + static_cast<unsigned>(direction)) * 8 + animationstep;
    return dynamic_cast<ArchivItem_Bitmap_Player*>(get(bodyIdx));
}

std::pair<ArchivItem_Bitmap_Player*, int16_t> ArchivItem_Bob::getOverlay(
    unsigned bobId, bool fat, ImgDir direction, unsigned animationstep)
{
    // Array [bobId][animStep][fat][direction]: [][8][2][6]
    const unsigned spriteId = ((bobId * 8 + animationstep) * 2 + fat) * 6 + static_cast<unsigned>(direction);
    auto* overlayImg = dynamic_cast<ArchivItem_Bitmap_Player*>(get(getOverlayIdx(spriteId)));
    return std::make_pair(overlayImg, spriteOverlayNyOffset[spriteId]);
}

void ArchivItem_Bob::writeLinks(std::ostream& file) const
{
    // spriteOverlayId[][8][2][6]
    for(const auto it : spriteOverlayIdx | boost::adaptors::indexed())
    {
        if(it.index() % NUM_LINKS_PER_OVERLAY == 0)
            file << "# Job ID " << it.index() / NUM_LINKS_PER_OVERLAY << "\n";
        file << s25util::toStringClassic(it.index()) << "\t" << s25util::toStringClassic(it.value()) << "\n";
    }
}

std::map<uint16_t, uint16_t> ArchivItem_Bob::readLinks(std::istream& file)
{
    std::map<uint16_t, uint16_t> result;
    loadMapping(file, [&result](unsigned idx, const std::string& value) {
        if(idx > std::numeric_limits<uint16_t>::max())
            throw std::range_error("Index " + std::to_string(idx) + " is to large");
        result[idx] = s25util::fromStringClassic<uint16_t>(value);
    });
    return result;
}

} // namespace libsiedler2
