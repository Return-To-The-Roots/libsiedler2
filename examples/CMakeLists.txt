# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

set(RTTR_PAL5_PATH "${CMAKE_CURRENT_SOURCE_DIR}/pal5.act")

include(GNUInstallDirs)

add_subdirectory(chTransparentIdx)
add_subdirectory(lstpacker)
add_subdirectory(outline)

install(FILES README.md DESTINATION .)
