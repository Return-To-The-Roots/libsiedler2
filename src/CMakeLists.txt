
FIND_PACKAGE(Boost 1.55.0 REQUIRED COMPONENTS filesystem iostreams system)

IF (MSVC)
	ADD_DEFINITIONS(/WX) # warning = error
	ADD_DEFINITIONS(/MP) # parallel compilation
	# disable MSVC posix functions
	ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
	ADD_DEFINITIONS(/wd"4250")
ENDIF()

get_filename_component(parentPath ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(includePath ${parentPath}/include)
INCLUDE_DIRECTORIES(${includePath}/libsiedler2 ${LIBENDIAN_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

FILE(GLOB SRC_FILES *.cpp *.h *.hpp)
FILE(GLOB HEADER_FILES ${includePath}/libsiedler2/*.*)
ADD_LIBRARY(siedler2 STATIC ${SRC_FILES} ${HEADER_FILES})

target_include_directories(siedler2 INTERFACE ${includePath})
TARGET_LINK_LIBRARIES(siedler2 s25util nowide-static ${Boost_LIBRARIES})
include(EnableWarnings)
enable_warnings(siedler2)

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES} ${HEADER_FILES})
endif()
