find_package(Boost 1.58 REQUIRED COMPONENTS program_options)

file(GLOB SOURCES *.cpp *.h)
add_executable(chTransparentIdx ${SOURCES})

target_link_libraries(chTransparentIdx siedler2 s25util ${Boost_LIBRARIES})

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
	if(CMAKE_VERSION VERSION_LESS 3.8)
		create_target_launcher(chTransparentIdx WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	else()
		set_property(TARGET chTransparentIdx PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif()
endif()

if(ClangFormat_FOUND)
    add_clangFormat_files(${SOURCES})
endif()
