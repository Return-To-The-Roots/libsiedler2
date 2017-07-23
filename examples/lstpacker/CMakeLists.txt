
file(GLOB LSTPACKER_SOURCES *.cpp *.h)
add_executable(lstpacker ${LSTPACKER_SOURCES})

target_link_libraries(lstpacker siedler2)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
	create_target_launcher(lstpacker WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
