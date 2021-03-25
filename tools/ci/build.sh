#!/bin/bash

set -euo pipefail

# Required by the tests
export USER="${USER:-TestUser}'"

BUILD_TYPE="${1}"
CMAKE_FLAGS="${2}"

mkdir build && cd build

if [[ "${TRAVIS_OS_NAME:-}" == "windows" ]] || [[ "${RUNNER_OS:-}" == "Windows" ]]; then
    GENERATOR="Visual Studio 16 2019"
    BUILD_FLAGS=""
else
    GENERATOR="Unix Makefiles"
    BUILD_FLAGS="-j3"
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
export BOOST_TEST_LOG_LEVEL=message
export BOOST_TEST_REPORT_LEVEL=detailed
ctest --build-config "${BUILD_TYPE}" --output-on-failure -j2
