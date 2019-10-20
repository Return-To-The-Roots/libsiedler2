# Project meant to be used as part of a super project including libutil and libendian
cmake_minimum_required(VERSION 3.8)
project(libsiedler2)

include(RttrBoostCfg)
find_package(Boost 1.64.0 REQUIRED COMPONENTS system filesystem iostreams)

file(GLOB _sources src/*.cpp)
file(GLOB_RECURSE _headers include/*.h include/*.hpp)
add_library(siedler2 STATIC ${_sources} ${_headers})

target_include_directories(siedler2 INTERFACE include PRIVATE include/libsiedler2)
target_link_libraries(siedler2 PUBLIC s25util::common PRIVATE nowide::static Boost::filesystem Boost::iostreams endian::interface)
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

include(RttrTestingCfg)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
