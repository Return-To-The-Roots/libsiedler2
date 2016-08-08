CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

project(lstpacker)

################################################################################
# Default boost libs are static on self-build msvc versions and dynamic in the linux package repos
IF(MSVC)
	SET(RTTR_USE_STATIC_BOOST_DEFAULT ON)
ELSE()
	SET(RTTR_USE_STATIC_BOOST_DEFAULT OFF)
ENDIF()
	
OPTION(RTTR_USE_STATIC_BOOST "Build with static BOOST libraries instead of dynamic" ${RTTR_USE_STATIC_BOOST_DEFAULT})
IF((NOT "${Boost_USE_STATIC_LIBS}" AND ${RTTR_USE_STATIC_BOOST}) OR
   ("${Boost_USE_STATIC_LIBS}" AND NOT ${RTTR_USE_STATIC_BOOST}))
    set(Boost_USE_STATIC_LIBS ${RTTR_USE_STATIC_BOOST})
    unset(Boost_INCLUDE_DIR CACHE)
    unset(Boost_LIBRARY_DIRS CACHE)
ENDIF()
if(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost static libraries")
else(Boost_USE_STATIC_LIBS)
   message(STATUS "Linking against boost dynamic libraries")
endif(Boost_USE_STATIC_LIBS)

# For some reason boost needs sometimes 1.xy and sometimes 1.xy.0 to detect folder structures like boost_1_55_0
SET(Boost_ADDITIONAL_VERSIONS "1.61.0" "1.61" "1.60.0" "1.60" "1.59.0" "1.59" "1.58.0" "1.58" "1.57.0" "1.57" "1.56.0" "1.56" "1.55.0" "1.55")

# Require higher version as libsiedler2 because endian functions are not emulated here
FIND_PACKAGE(Boost 1.58.0 REQUIRED filesystem system)

IF (MSVC)
    ADD_DEFINITIONS(/WX) # warning = error
    ADD_DEFINITIONS(/MP) # parallel compilation
    # disable MSVC posix functions
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(/wd"4250")
ENDIF()

IF (NOT "${CMAKE_CXX_STANDARD}" EQUAL "11" AND NOT MSVC)
    ADD_DEFINITIONS(-Doverride=)
ENDIF()

get_filename_component(LIB_SIEDLER2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../.. ABSOLUTE)

add_subdirectory(${LIB_SIEDLER2_DIR}/../libendian ${CMAKE_CURRENT_BINARY_DIR}/libendian)
add_subdirectory(${LIB_SIEDLER2_DIR} ${CMAKE_CURRENT_BINARY_DIR}/libsiedler2)

include_directories(${LIB_SIEDLER2_DIR}/src)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} lstpacker.cpp pack.cpp unpack.cpp util.h)

target_link_libraries(${PROJECT_NAME} siedler2)

