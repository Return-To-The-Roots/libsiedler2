
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

if(NOT Boost_USE_STATIC_LIBS)
	ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK) 
endif()

if(MSVC)
    # Use full paths for defines (makes Boost.Test with the VS addin work better)
    add_definitions(/FC)
endif()

file(GLOB TEST_SOURCES *.cpp *.h)
add_executable(Test ${TEST_SOURCES})
target_link_libraries(Test siedler2	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME MainTest COMMAND Test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)

if(MSVC)
	create_target_launcher(Test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
