// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "oem.h"
#include <array>

namespace libsiedler2 {

/// Konvertiertabelle von ANSI nach OEM, beginnend bei char 128
static const std::array<uint8_t, 128> ansi2oem_tab = {
  /*0080:*/ 0x00, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x00, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /*0090:*/ 0x00, 0x60, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  /*00A0:*/ 0xFF, 0xAD, 0x9B, 0x9C, 0x0F, 0x9D, 0x7C, 0x15, 0x22, 0x63, 0xA6, 0xAE, 0xAA, 0x2D, 0x52, 0x00,
  /*00B0:*/ 0xF8, 0xF1, 0xFD, 0x33, 0x27, 0xE6, 0x14, 0xFA, 0x2C, 0x31, 0xA7, 0xAF, 0xAC, 0xAB, 0x00, 0xA8,
  /*00C0:*/ 0x41, 0x41, 0x41, 0x41, 0x8E, 0x8F, 0x92, 0x80, 0x45, 0x90, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49,
  /*00D0:*/ 0x44, 0xA5, 0x4F, 0x4F, 0x4F, 0x4F, 0x99, 0x78, 0x4F, 0x55, 0x55, 0x55, 0x9A, 0x59, 0x00, 0xE1,
  /*00E0:*/ 0x85, 0xA0, 0x83, 0x61, 0x84, 0x86, 0x91, 0x87, 0x8A, 0x82, 0x88, 0x89, 0x8D, 0xA1, 0x8C, 0x8B,
  /*00F0:*/ 0x64, 0xA4, 0x95, 0xA2, 0x93, 0x6F, 0x94, 0xF6, 0x6F, 0x97, 0xA3, 0x96, 0x81, 0x79, 0x00, 0x98,
};

/** @name AnsiToOem
 *
 *  Wandelt einen String vom ANSI ins OEM Format um.
 *
 *  @param[in]  from   konstanter Quellstring
 *  @param[out] to     Zielstring (ausreichend Speicher muss vorhanden sein)
 *  @param[in]  length Länge des Quellstrings, wenn 0 wird @p strlen(from) verwendet
 *
 *  @return            @p to wird zurückgeliefert
 */
std::string AnsiToOem(const std::string& from)
{
    std::string result(from.size(), '\0');

    // und string umwandeln
    for(uint32_t x = 0; x < from.size(); x++)
    {
        auto C = (uint8_t)from[x];

        // ab char 128 nötig
        if(C > 128)
            result[x] = (char)ansi2oem_tab[C & 0x7F];
        else
            result[x] = from[x];
    }
    return result;
}

/** @name OemToAnsi
 *
 *  Wandelt einen String vom OEM ins ANSI Format um.
 *
 *  @param[in]  from   konstanter Quellstring
 *  @param[out] to     Zielstring (ausreichend Speicher muss vorhanden sein)
 *  @param[in]  length Länge des Quellstrings, wenn 0 wird @p strlen(from) verwendet
 *
 *  @return            @p to wird zurückgeliefert
 */
std::string OemToAnsi(const std::string& from)
{
    std::string result(from.size(), '\0');

    // und string umwandeln
    for(uint32_t x = 0; x < from.size(); ++x)
    {
        if((uint8_t)from[x] > 128)
        {
            for(int i = 0x83; i < 256; ++i)
            {
                if((uint8_t)from[x] == ansi2oem_tab[i - 0x80])
                {
                    result[x] = (char)i;
                    break;
                }
            }
        } else
            result[x] = from[x];
    }
    return result;
}

} // namespace libsiedler2
