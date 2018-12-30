cmake_minimum_required(VERSION 3.8)

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

list(APPEND CMAKE_MODULE_PATH "${RTTR_LIBUTIL_DIR}/cmake")

include(CheckCXXSourceCompiles)
include(AddFlags)
enable_testing()

################################################################################
include(RttrBoostCfg)

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

add_subdirectory(${RTTR_LIBUTIL_DIR} libutil)
add_subdirectory(${RTTR_LIBENDIAN_DIR} libEndian)
add_subdirectory(${RTTR_LIBSIEDLER2_DIR} libSiedler2)

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(RTTR_PAL5_PATH "${RTTR_LIBSIEDLER2_DIR}/examples/pal5.act")

add_subdirectory(chTransparentIdx)
add_subdirectory(lstpacker)
add_subdirectory(outline)
add_subdirectory(test)

if(ClangFormat_FOUND)
    add_clangFormat_target("file")
endif()
