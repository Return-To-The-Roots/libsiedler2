CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

include(CheckCXXSourceCompiles)

project(libSiedler2)

enable_testing()

################################################################################
# Default boost libs are static on self-build msvc versions and dynamic in the linux package repos
IF(MSVC)
	SET(RTTR_USE_STATIC_BOOST_DEFAULT ON)
ELSE()
	SET(RTTR_USE_STATIC_BOOST_DEFAULT OFF)
ENDIF()
	
OPTION(RTTR_USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" ${RTTR_USE_STATIC_BOOST_DEFAULT})
IF((NOT "${Boost_USE_STATIC_LIBS}" AND ${RTTR_USE_STATIC_BOOST}) OR
   ("${Boost_USE_STATIC_LIBS}" AND NOT ${RTTR_USE_STATIC_BOOST}))
    set(Boost_USE_STATIC_LIBS ${RTTR_USE_STATIC_BOOST})
    unset(Boost_INCLUDE_DIR CACHE)
    unset(Boost_LIBRARY_DIRS CACHE)
ENDIF()
if(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost static libraries")
else()
   message(STATUS "Linking against boost dynamic libraries")
endif()

# For some reason boost needs sometimes 1.xy and sometimes 1.xy.0 to detect folder structures like boost_1_55_0
SET(TMP_VERSIONS "1.64" "1.63" "1.62" "1.61" "1.60" "1.59" "1.58")
foreach(I_VERSION ${TMP_VERSIONS})
	list(APPEND Boost_ADDITIONAL_VERSIONS "${I_VERSION}.0" "${I_VERSION}")
endforeach()
UNSET(TMP_VERSIONS)

# Require higher version as libsiedler2 because endian functions are not emulated here
FIND_PACKAGE(Boost 1.58.0 REQUIRED filesystem system)

OPTION(RTTR_ENABLE_WERROR "Build with warnings turned into errors" ON)

IF (MSVC)
	IF(RTTR_ENABLE_WERROR)
		ADD_DEFINITIONS(/WX)
	ENDIF()
    ADD_DEFINITIONS(/MP) # parallel compilation
    # disable MSVC posix functions
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(/wd"4250")
else()
    ADD_DEFINITIONS(-Wall)
	IF(RTTR_ENABLE_WERROR)
		ADD_DEFINITIONS(-Werror)
	ENDIF()
ENDIF()

CHECK_CXX_SOURCE_COMPILES("
	#if __cplusplus >= 201103L
	int main() {}
	#endif" COMPILER_IN_CXX11_MODE)

IF ((NOT COMPILER_IN_CXX11_MODE) AND NOT MSVC)
    ADD_DEFINITIONS(-Doverride=)
ENDIF()

# VS does not support coverage analysis
if(NOT MSVC)
	option(RTTR_ENABLE_COVERAGE "Generate coverage build" OFF)
	if(RTTR_ENABLE_COVERAGE)
		if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			message(FATAL_ERROR "Coverage requires a debug build or you get false positives")
		endif()
		# Note: "--coverage" instead of "-coverage" makes this work with ccache
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 --coverage")
		if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
			# Inlining makes the coverage statistic much harder to read and may lead to lots of partials
			# However expect a slowdown!
			message(STATUS "Enabled no-inline for better coverage statistics")
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-inline -fno-inline-small-functions -fno-default-inline")
		else()
			message(STATUS "Using default inlining which probably influences coverage statistics")
		endif()
	endif()
endif()

find_path(LIB_SIEDLER2_BASEDIR
	NAMES "libsiedler2/src/libsiedler2.h"
	PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..)
if(NOT LIB_SIEDLER2_BASEDIR)
    message(FATAL_ERROR "Path to libsiedler2 not found!")
endif()

list(APPEND CMAKE_MODULE_PATH "${LIB_SIEDLER2_BASEDIR}/libSiedler2/cmake/Modules")

if(MSVC)
	CMAKE_POLICY(SET CMP0026 OLD) # Required for use of LOCATION_*
    INCLUDE(CreateLaunchers)
endif()

find_path(LIB_ENDIAN_BASEDIR
	NAMES "libendian/src/libendian.h"
	PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../.. ENV LIB_ENDIAN_DIR
    PATH_SUFFIXES ../)
if(NOT LIB_ENDIAN_BASEDIR)
    message(FATAL_ERROR "Path to libendian not found! You can set the full path in the env var LIB_ENDIAN_DIR ($ENV{LIB_ENDIAN_DIR})")
endif()

include_directories(${LIB_ENDIAN_BASEDIR})

#add_subdirectory(${LIB_SIEDLER2_DIR}/../libendian ${CMAKE_CURRENT_BINARY_DIR}/libendian)
add_subdirectory(${LIB_SIEDLER2_BASEDIR}/libsiedler2 ${CMAKE_CURRENT_BINARY_DIR}/libsiedler2)

include_directories(${LIB_SIEDLER2_BASEDIR})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(RTTR_PAL5_PATH "${LIB_SIEDLER2_BASEDIR}/libsiedler2/examples/pal5.act")

add_subdirectory(lstpacker)
add_subdirectory(outline)
add_subdirectory(test)