# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

add_executable(outline outline.cpp)
target_link_libraries(outline siedler2)

file(COPY ${RTTR_PAL5_PATH} DESTINATION "${CMAKE_BINARY_DIR}")
install(FILES ${RTTR_PAL5_PATH} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS outline DESTINATION ${CMAKE_INSTALL_BINDIR})

if(MSVC)
    set_property(TARGET outline PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

include(EnableWarnings)
enable_warnings(outline)

if(ClangFormat_FOUND)
    add_clangFormat_files(outline.cpp)
endif()
