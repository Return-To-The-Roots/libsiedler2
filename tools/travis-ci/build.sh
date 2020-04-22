#!/bin/bash

set -euo pipefail

BUILD_TYPE="${1}"
CMAKE_FLAGS="${2}"

mkdir build && cd build

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    GENERATOR="Visual Studio 15 2017 Win64"
    BUILD_FLAGS=""
else
    GENERATOR="Unix Makefiles"
    # Travis uses 2 cores
    BUILD_FLAGS="-j2"
fi

cmake \
    --generator="$GENERATOR" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DRTTR_ENABLE_WERROR=ON \
    -DCMAKE_INSTALL_PREFIX="$PWD/installed" \
    ${CMAKE_FLAGS} \
    ..

cmake --build . --target install --config "${BUILD_TYPE}" -- $BUILD_FLAGS

# Execute tests
export RTTR_DISABLE_ASSERT_BREAKPOINT=1
ctest --build-config "${BUILD_TYPE}" --output-on-failure -j2
