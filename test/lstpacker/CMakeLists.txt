CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

get_filename_component(LIB_SIEDLER2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

project(lstpacker)

# Require higher version as libsiedler2 because endian functions are not emulated here
FIND_PACKAGE(Boost 1.58.0)

IF (MSVC)
    ADD_DEFINITIONS(/WX) # warning = error
    ADD_DEFINITIONS(/MP) # parallel compilation
    # disable MSVC posix functions
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF()

IF (NOT "${CMAKE_CXX_STANDARD}" EQUAL "11" AND NOT MSVC)
    ADD_DEFINITIONS(-Doverride=)
ENDIF()

add_subdirectory(${LIB_SIEDLER2_DIR}/../libendian ${CMAKE_CURRENT_BINARY_DIR}/libendian)
add_subdirectory(${LIB_SIEDLER2_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libsiedler2)

include_directories(${LIB_SIEDLER2_DIR}/src)

add_executable(${PROJECT_NAME} lstpacker.cpp pack.cpp unpack.cpp util.h)

target_link_libraries(${PROJECT_NAME} siedler2)
