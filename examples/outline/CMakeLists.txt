add_executable(outline outline.cpp)
target_link_libraries(outline siedler2)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
    set_property(TARGET outline PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(EnableWarnings)
enable_warnings(outline)

if(ClangFormat_FOUND)
    add_clangFormat_files(${_sources})
endif()
