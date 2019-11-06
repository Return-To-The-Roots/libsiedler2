# RTTR Utilities for The Settlers 2

## LstPacker
Packs and unpacks file archives from The Settlers 2.
This is mostly meant for use with LST files, which are simple collections of (mainly) images.
But it also works for other archive-like files, like Font-, Sound- and Palette-Files.

See `lstpacker --help` for details on usage but mostly it is `lstpacker <foo.lst>` for unpacking and `lstpacker <foo>` for packing a folder into a lst file.

## ChTransparentIdx
The image files have a special palette index that is used as the transparent color.
With this tool you can change the index to another index.

Use `chTransparentIdx --from <254> --to <255> <foo.lst>` to change all images in that lst file.

## Outline
This is an internal helper tool to extract the fonts from the `RESOURCE.DAT` file and convert it into a font file usable by RTTR.
Is is only kept for reference and no longer actually used.
