find_package(Boost 1.64 REQUIRED COMPONENTS system program_options)

add_executable(chTransparentIdx chTransparentIdx.cpp)

target_link_libraries(chTransparentIdx siedler2 nowide::static Boost::program_options)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)
install(TARGETS chTransparentIdx DESTINATION ${CMAKE_INSTALL_BINDIR})

if(MSVC)
    set_property(TARGET chTransparentIdx PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(EnableWarnings)
enable_warnings(chTransparentIdx)

if(ClangFormat_FOUND)
    add_clangFormat_files(${_sources})
endif()
