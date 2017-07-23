
FIND_PACKAGE(Boost 1.55.0 COMPONENTS filesystem iostreams system REQUIRED)

IF (MSVC)
	ADD_DEFINITIONS(/WX) # warning = error
	ADD_DEFINITIONS(/MP) # parallel compilation
	# disable MSVC posix functions
	ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
	ADD_DEFINITIONS(/wd"4250")
ENDIF()

ADD_DEFINITIONS(-DNOMINMAX)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})

FILE(GLOB SRC_FILES *.cpp *.h *.hpp)
ADD_LIBRARY(siedler2 STATIC ${SRC_FILES})

TARGET_LINK_LIBRARIES(siedler2 ${Boost_LIBRARIES})

#################################################################################
