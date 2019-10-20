# Example project using libsiedler2 creating useful tools
cmake_minimum_required(VERSION 3.8)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
project(libsiedler2-examples)

set(RTTR_LIBSIEDLER2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_path(RTTR_LIBUTIL_DIR NAMES "libs/common/include/s25util" HINTS ${RTTR_LIBSIEDLER2_DIR}/../libutil PATHS ENV LIBUTIL_DIR PATH_SUFFIXES ".." libutil)
if(NOT RTTR_LIBUTIL_DIR)
    message(FATAL_ERROR "Path to libutil not found! You can set the full path in the env var LIBUTIL_DIR ($ENV{LIBUTIL_DIR})")
endif()

find_path(RTTR_LIBENDIAN_DIR NAMES "include/libendian" HINTS ${RTTR_LIBSIEDLER2_DIR}/../libendian PATHS ENV LIBENDIAN_DIR PATH_SUFFIXES ".." libendian)
if(NOT RTTR_LIBENDIAN_DIR)
    message(FATAL_ERROR "Path to libendian not found! You can set the full path in the env var LIBENDIAN_DIR ($ENV{LIBENDIAN_DIR})")
endif()

list(APPEND CMAKE_MODULE_PATH "${RTTR_LIBUTIL_DIR}/cmake")

# include Test
include(RttrCoverageCfg)

# Use clang-format if available
find_package(ClangFormat 5.0.0)

# Must be done at top-level
include(RttrTestingCfg)

add_subdirectory(${RTTR_LIBUTIL_DIR} libutil)
add_subdirectory(${RTTR_LIBENDIAN_DIR} libendian)
add_subdirectory(${RTTR_LIBSIEDLER2_DIR} libsiedler2)

set(RTTR_PAL5_PATH "${RTTR_LIBSIEDLER2_DIR}/examples/pal5.act")

include(GNUInstallDirs)

add_subdirectory(chTransparentIdx)
add_subdirectory(lstpacker)
add_subdirectory(outline)

if(ClangFormat_FOUND)
    add_clangformat_target("file")
endif()
