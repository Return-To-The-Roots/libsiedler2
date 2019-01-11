find_package(Boost 1.64.0 REQUIRED COMPONENTS filesystem iostreams)

get_filename_component(_parent_dir ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(_include_dir ${_parent_dir}/include)

file(GLOB _sources *.cpp)
file(GLOB _headers ${_include_dir}/libsiedler2/*.h)
add_library(siedler2 STATIC ${_sources} ${_headers})

target_include_directories(siedler2 INTERFACE ${_include_dir} PRIVATE ${_include_dir}/libsiedler2)
target_link_libraries(siedler2 PUBLIC s25util PRIVATE nowide::static Boost::filesystem Boost::iostreams endian::interface)
target_compile_features(siedler2 PUBLIC cxx_std_14)
set_target_properties(siedler2 PROPERTIES CXX_EXTENSIONS OFF)

include(EnableWarnings)
enable_warnings(siedler2)

if(ClangFormat_FOUND)
    add_clangformat_files(${_sources} ${_headers})
endif()
