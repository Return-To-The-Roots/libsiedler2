#################################################################################
### $Id: CMakeLists.txt 7505 2011-09-07 13:14:38Z FloSoft $
#################################################################################

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/libendian/src")
FIND_PACKAGE(Boost 1.57.0 REQUIRED)

IF (MSVC)
	ADD_DEFINITIONS(/WX) # warning = error
	ADD_DEFINITIONS(/MP) # parallel compilation
	# disable MSVC posix functions
	ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF()

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(siedler2 libsiedler2.cpp ArchivInfo.cpp ArchivItem.cpp
            ArchivItem_Bitmap.cpp ArchivItem_Bitmap_Player.cpp
            ArchivItem_Bitmap_Raw.cpp ArchivItem_Bitmap_RLE.cpp
            ArchivItem_Bitmap_Shadow.cpp ArchivItem_Bob.cpp ArchivItem_Font.cpp
            ArchivItem_Map.cpp ArchivItem_Map_Header.cpp ArchivItem_Palette.cpp
            ArchivItem_Sound.cpp ArchivItem_Sound_XMidi.cpp ArchivItem_Sound_Midi.cpp 
            ArchivItem_Sound_Wave.cpp ArchivItem_Sound_Other.cpp
            ArchivItem_Text.cpp ArchivItem_Raw.cpp oem.cpp LoadType.cpp WriteType.cpp
            ArchivItem_Ini.cpp
            LoadACT.cpp WriteACT.cpp LoadBBM.cpp WriteBBM.cpp LoadBMP.cpp
            WriteBMP.cpp LoadBOB.cpp WriteBOB.cpp LoadDATIDX.cpp WriteDATIDX.cpp
            LoadLBM.cpp WriteLBM.cpp LoadLST.cpp WriteLST.cpp LoadMAP.cpp
            WriteMAP.cpp LoadTLST.cpp LoadTXT.cpp WriteTXT.cpp LoadSND.cpp MIDI_Track.cpp
            WriteINI.cpp LoadINI.cpp 
            XMIDI_Track.cpp)

TARGET_LINK_LIBRARIES(siedler2 endian)

#################################################################################
