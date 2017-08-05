
FIND_PACKAGE(Boost 1.55.0 COMPONENTS filesystem iostreams system REQUIRED)

IF (MSVC)
	ADD_DEFINITIONS(/WX) # warning = error
	ADD_DEFINITIONS(/MP) # parallel compilation
	# disable MSVC posix functions
	ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
	ADD_DEFINITIONS(/wd"4250")
else()
    include(CheckCXXSourceCompiles)
    CHECK_CXX_SOURCE_COMPILES("
        #if __cplusplus >= 201103L
        int main() {}
        #endif" COMPILER_IN_CXX11_MODE)

    IF (NOT COMPILER_IN_CXX11_MODE)
        ADD_DEFINITIONS(-Doverride=)
    ENDIF()
ENDIF()

ADD_DEFINITIONS(-DNOMINMAX)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

FILE(GLOB SRC_FILES *.cpp *.h *.hpp)
ADD_LIBRARY(siedler2 STATIC ${SRC_FILES})

TARGET_LINK_LIBRARIES(siedler2 ${Boost_LIBRARIES})

if(ClangFormat_FOUND)
    add_clangFormat_files(${SRC_FILES})
endif()

#################################################################################
