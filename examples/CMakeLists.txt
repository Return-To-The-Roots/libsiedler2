CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(siedler2Examples)

set(RTTR_LIBSIEDLER2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_path(RTTR_LIBUTIL_DIR
	NAMES "include/libutil"
    HINTS ${RTTR_LIBSIEDLER2_DIR}/../libutil
	PATHS ENV LIBUTIL_DIR
    PATH_SUFFIXES ".." libutil)
if(NOT RTTR_LIBUTIL_DIR)
    message(FATAL_ERROR "Path to libutil not found! You can set the full path in the env var LIBUTIL_DIR ($ENV{LIBUTIL_DIR})")
endif()

find_path(RTTR_LIBENDIAN_DIR
	NAMES "include/libendian"
    HINTS ${RTTR_LIBSIEDLER2_DIR}/../libendian
	PATHS ENV LIBENDIAN_DIR
    PATH_SUFFIXES ".." libendian)
if(NOT RTTR_LIBENDIAN_DIR)
    message(FATAL_ERROR "Path to libendian not found! You can set the full path in the env var LIBENDIAN_DIR ($ENV{LIBENDIAN_DIR})")
endif()

list(APPEND CMAKE_MODULE_PATH "${RTTR_LIBUTIL_DIR}/cmake/Modules")

include(CheckCXXSourceCompiles)
include(CMakeMacroAddFlags)
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
		add_compile_options(/WX)
	ENDIF()
    add_compile_options(/MP) # parallel compilation
    # disable MSVC posix functions
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(/wd"4250")
else()
    add_compile_options(-Wall)
	IF(RTTR_ENABLE_WERROR)
		add_compile_options(-Werror)
	ENDIF()
ENDIF()

if(WIN32)
    ADD_DEFINITIONS(-DNOMINMAX)
endif()

if(NOT MSVC)
    CHECK_CXX_SOURCE_COMPILES("
        #if __cplusplus >= 201103L
        int main() {}
        #endif" COMPILER_IN_CXX11_MODE)

    IF (NOT COMPILER_IN_CXX11_MODE)
        ADD_DEFINITIONS(-Doverride=)
    endif()
ENDIF()

################################################################################
# Code coverage
################################################################################

# VS does not support coverage analysis
if(NOT MSVC)
	option(RTTR_ENABLE_COVERAGE "Generate coverage build" OFF)
	if(RTTR_ENABLE_COVERAGE)
		if(NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
			message(FATAL_ERROR "Coverage requires a debug build or you get false positives")
		endif()
		# Note: "--coverage" instead of "-coverage" makes this work with ccache
		add_flags(CMAKE_CXX_FLAGS_DEBUG -O0 --coverage)
		if(CMAKE_COMPILER_IS_GNUCXX)
			# Inlining makes the coverage statistic much harder to read and may lead to lots of partials
			# However expect a slowdown!
			message(STATUS "Enabled no-inline for better coverage statistics")
			add_flags(CMAKE_CXX_FLAGS_DEBUG -fno-inline -fno-inline-small-functions -fno-default-inline)
		else()
			message(STATUS "Using default inlining which probably influences coverage statistics")
		endif()
	endif()
endif()


# Use clang-format if available
find_package(ClangFormat 5.0.0)

if(MSVC AND CMAKE_VERSION VERSION_LESS 3.8)
	CMAKE_POLICY(SET CMP0026 OLD) # Required for use of LOCATION_*
    INCLUDE(CreateLaunchers)
endif()

set(NOWIDE_BUILD_STATIC ON CACHE BOOL "" FORCE)
set(NOWIDE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(NOWIDE_USE_BOOST_FILESYSTEM ON CACHE BOOL "" FORCE)
add_subdirectory(${RTTR_LIBUTIL_DIR}/nowide/src nowide)
add_subdirectory(${RTTR_LIBENDIAN_DIR} libEndian)
add_subdirectory(${RTTR_LIBSIEDLER2_DIR} libSiedler2)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(RTTR_PAL5_PATH "${RTTR_LIBSIEDLER2_DIR}/examples/pal5.act")

add_subdirectory(lstpacker)
add_subdirectory(outline)
add_subdirectory(test)

if(ClangFormat_FOUND)
    add_clangFormat_target("file")
endif()