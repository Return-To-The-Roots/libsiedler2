#!/bin/bash

set -euox pipefail

BUILD_TYPE=${1}
CMAKE_FLAGS=${2}

mkdir build && cd build
cmake \
    --generator="Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DRTTR_ENABLE_WERROR=ON \
    ${CMAKE_FLAGS} \
    ../examples

# Travis uses 2 cores
make -j2

# Execute tests
export CTEST_OUTPUT_ON_FAILURE=1
export BOOST_TEST_CATCH_SYSTEM_ERRORS="no"
export RTTR_DISABLE_ASSERT_BREAKPOINT=1

make test
