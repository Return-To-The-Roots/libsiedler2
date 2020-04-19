include(AddTestcase)

get_filename_component(RTTR_LIBSIEDLER2_TEST_FILES_DIR "testFiles" ABSOLUTE)
configure_file(config.h.cmake include/test/config.h @ONLY)
unset(RTTR_LIBSIEDLER2_TEST_FILES_DIR)

add_library(siedler2-testConfig INTERFACE)
target_include_directories(siedler2-testConfig INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/include)

add_testcase(NAME libsiedler2 LIBS siedler2 siedler2-testConfig)

if(ClangFormat_FOUND)
    get_target_property(TEST_SOURCES Test_libsiedler2 SOURCES)
    add_clangFormat_files(${TEST_SOURCES})
endif()
