
add_executable(outline outline.cpp)

target_link_libraries(outline siedler2)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
	if(CMAKE_VERSION VERSION_LESS 3.8)
		create_target_launcher(outline WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	else()
		set_property(TARGET outline PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif()
endif()

if(ClangFormat_FOUND)
    add_clangFormat_files(outline.cpp)
endif()
