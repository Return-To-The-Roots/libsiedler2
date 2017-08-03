# libSiedler2 - Library for reading and writing 'The Settlers II' file formats


### Texturformat
The bitmaps can be stored in memory in ARGB or paletted format.
The ARGB format is byte BGRA, so 4 bytes define 1 pixel with blue first and alpha last.
If this is interpreted as a 32bit word on a little endian machine than it is (word) ARGB format.
That is the alpha value is the most significant byte.
The used format can be set with `setTextureFormat`.
All bitmaps loaded **after** this call will be in the given format with conversion beeing used if required.   
Note: While it is possible to convert Paletted -> ARGB in all cases the reverse is not always possible.
So using a paletted format may result in failure of loading ARGB bitmaps.