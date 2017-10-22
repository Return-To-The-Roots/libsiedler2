
find_package(Boost REQUIRED COMPONENTS unit_test_framework)

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
set(RTTR_TESTFILES "${CMAKE_CURRENT_SOURCE_DIR}/testFiles")
file(COPY ${RTTR_TESTFILES} DESTINATION "${CMAKE_BINARY_DIR}")
install(DIRECTORY ${RTTR_TESTFILES} DESTINATION .)

if(MSVC)
	if(CMAKE_VERSION VERSION_LESS 3.8)
		create_target_launcher(Test WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	else()
		set_property(TARGET Test VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	endif()
endif()

if(ClangFormat_FOUND)
    add_clangFormat_files(${TEST_SOURCES})
endif()
