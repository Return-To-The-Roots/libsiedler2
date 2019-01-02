include(AddMacros)

add_testcase(LibSiedler2 LIBS siedler2)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION .)
set(RTTR_TESTFILES "${CMAKE_CURRENT_SOURCE_DIR}/testFiles")
file(COPY ${RTTR_TESTFILES} DESTINATION "${CMAKE_BINARY_DIR}")
install(DIRECTORY ${RTTR_TESTFILES} DESTINATION .)

if(ClangFormat_FOUND)
    add_clangFormat_files(${TEST_SOURCES})
endif()
