find_package(Boost 1.64 REQUIRED COMPONENTS program_options)

file(GLOB _sources *.cpp *.h)
add_executable(chTransparentIdx ${_sources})

target_link_libraries(chTransparentIdx siedler2 s25util Boost::program_options)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
    set_property(TARGET chTransparentIdx PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(EnableWarnings)
enable_warnings(chTransparentIdx)

if(ClangFormat_FOUND)
    add_clangFormat_files(${_sources})
endif()
