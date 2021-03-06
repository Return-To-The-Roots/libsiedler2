#!/bin/bash

# Copyright (C) 2005 - 2021 Settlers Freaks <sf-team at siedler25.org>
#
# SPDX-License-Identifier: GPL-2.0-or-later

set -euo pipefail

CC="$COMPILER"

if [[ "$CC" =~ clang ]]; then
  CXX=${CC/clang/clang++}
elif [[ "$CC" =~ gcc ]]; then
  CXX=${CC/gcc/g++}
fi
ver=7 # default
if [[ "$CC" =~ gcc- ]]; then
  ver="${CC##*gcc-}"
fi
GCOV=gcov-${ver}
echo "CC=$CC" >> $GITHUB_ENV
echo "CXX=$CXX" >> $GITHUB_ENV
echo "GCOV=$GCOV" >> $GITHUB_ENV
