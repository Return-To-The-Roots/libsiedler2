# libsiedler2 - Library for reading and writing 'The Settlers II' file formats

# Current Build Info

Travis CI: ![Travis CI Build Info](https://travis-ci.org/Return-To-The-Roots/libsiedler2.svg?branch=master)

Appveyor: coming...

Coverage:
[![Coverage Status](https://coveralls.io/repos/github/Return-To-The-Roots/libsiedler2/badge.svg?branch=master)](https://coveralls.io/github/Return-To-The-Roots/libsiedler2?branch=master)
[![codecov](https://codecov.io/gh/Return-To-The-Roots/libsiedler2/branch/master/graph/badge.svg)](https://codecov.io/gh/Return-To-The-Roots/libsiedler2)


### Texture format
The bitmaps can be stored in memory in ARGB or paletted format.
The ARGB format is byte BGRA, so 4 bytes define 1 pixel with blue first and alpha last.
If this is interpreted as a 32bit word on a little endian machine than it is (word) ARGB format.
That is the alpha value is the most significant byte.
The used format can be set with `setTextureFormat`.
All bitmaps loaded **after** this call will be in the given format with conversion beeing used if required.   
Note: While it is possible to convert Paletted -> ARGB in all cases the reverse is not always possible.
So using a paletted format may result in failure of loading ARGB bitmaps.

### Palettes
As some files may contain embedded palettes care must be taken when reading, writing or using them.

If the bitmap is paletted, it contains a palette.  
If it is ARGB it may contain a palette and this palette may be invalid (not contain all colors) which can result in errors when converting to paletted.
Use `checkPalette` to make sure the palette is valid. (Trivially true for paletted bitmaps.)

For the usage the following applies:

1. No palette is required or used when no conversion occurs (Read/Write, Create/Print to from file/buffer with same format)
2. Loading: If the file contains a palette, it is stored in the bitmap and used.   
            If the file does not contain a palette the passed palette is used, but only stored with the bitmap if it is paletted.
3. Writing: If the file format contains a palette and the bitmap contains one, it is used instead of the passed palette.
            Otherwise the palette passed is used with the bitmaps palette as a fallback.
            It is an error not to pass a palette when conversion is required unless the bitmap contains one.   
            If the file format supports paletted and unpalleted images then the paletted format is used, if (and only if) the bitmap is paletted or contains a palette.
4. Drawing: The `print` function will prefer the passed palette for conversion from ARGB and the contained palette for conversion to ARGB.
5. Creating: The `create` function resets the bitmap completely (size, palette, format, data).
             It will create a bitmap with the same format as the buffer.
             A palette is required and stored for paletted formats.
