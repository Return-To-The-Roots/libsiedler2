# Project meant to be used as part of a super project including libutil and libendian
cmake_minimum_required(VERSION 3.9)
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()
project(libsiedler2)

# If top-level project
if(PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(isTopLevel TRUE)
    find_path(RTTR_LIBUTIL_DIR NAMES "libs/common/include/s25util" HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../libutil PATHS ENV LIBUTIL_DIR PATH_SUFFIXES ".." libutil)
    if(NOT RTTR_LIBUTIL_DIR)
        message(FATAL_ERROR "Path to libutil not found! You can set the full path in the env var LIBUTIL_DIR ($ENV{LIBUTIL_DIR})")
    endif()

    find_path(RTTR_LIBENDIAN_DIR NAMES "include/libendian" HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../libendian PATHS ENV LIBENDIAN_DIR PATH_SUFFIXES ".." libendian)
    if(NOT RTTR_LIBENDIAN_DIR)
        message(FATAL_ERROR "Path to libendian not found! You can set the full path in the env var LIBENDIAN_DIR ($ENV{LIBENDIAN_DIR})")
    endif()

    list(APPEND CMAKE_MODULE_PATH "${RTTR_LIBUTIL_DIR}/cmake")
    include(RttrCoverageCfg)
    # Use clang-format if available
    find_package(ClangFormat 9)
else()
    set(isTopLevel FALSE)
endif()


include(RttrBoostCfg)
find_package(Boost 1.64.0 REQUIRED COMPONENTS system filesystem iostreams)

include(RttrTestingCfg)
if(isTopLevel)
    add_subdirectory(${RTTR_LIBUTIL_DIR} libutil)
    add_subdirectory(${RTTR_LIBENDIAN_DIR} libendian)
endif()

file(GLOB _sources src/*.cpp)
file(GLOB_RECURSE _headers include/*.h include/*.hpp)
add_library(siedler2 STATIC ${_sources} ${_headers})

target_include_directories(siedler2 INTERFACE include PRIVATE include/libsiedler2)
target_link_libraries(siedler2 PUBLIC s25util::common PRIVATE Boost::nowide Boost::filesystem Boost::iostreams endian::interface)
target_compile_features(siedler2 PUBLIC cxx_std_14)
set_target_properties(siedler2 PROPERTIES CXX_EXTENSIONS OFF)
if(WIN32)
    target_compile_definitions(siedler2 PUBLIC NOMINMAX)
endif()

include(EnableWarnings)
enable_warnings(siedler2)

if(ClangFormat_FOUND)
    add_clangformat_files(${_sources} ${_headers})
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

option(RTTR_BUILD_LIBSIEDLER2_EXAMPLES "Build example projects/tools of libsiedler2" ${isTopLevel})
if(RTTR_BUILD_LIBSIEDLER2_EXAMPLES)
    add_subdirectory(examples)
endif()
