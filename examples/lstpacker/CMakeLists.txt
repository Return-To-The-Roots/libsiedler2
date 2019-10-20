find_package(Boost 1.64.0 REQUIRED COMPONENTS system program_options)

file(GLOB _sources *.cpp *.h)
add_executable(lstpacker ${_sources})

target_link_libraries(lstpacker siedler2 nowide::static Boost::program_options)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
    set_property(TARGET lstpacker PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(EnableWarnings)
enable_warnings(lstpacker)

if(ClangFormat_FOUND)
    add_clangFormat_files(${_sources})
endif()
