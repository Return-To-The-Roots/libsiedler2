find_package(Boost 1.58 REQUIRED COMPONENTS program_options)

file(GLOB LSTPACKER_SOURCES *.cpp *.h)
add_executable(lstpacker ${LSTPACKER_SOURCES})

target_link_libraries(lstpacker siedler2 ${Boost_LIBRARIES})

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
	create_target_launcher(lstpacker WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

if(ClangFormat_FOUND)
    add_clangFormat_files(${LSTPACKER_SOURCES})
endif()
