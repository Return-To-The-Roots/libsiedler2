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

FILE(GLOB SRC_FILES *.cpp *.h)
ADD_LIBRARY(siedler2 STATIC ${SRC_FILES})

TARGET_LINK_LIBRARIES(siedler2 endian)

#################################################################################
