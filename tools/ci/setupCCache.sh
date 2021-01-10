#!/bin/bash

set -euo pipefail

echo "CCACHE_BASEDIR=$PWD" >> $GITHUB_ENV
echo "CCACHE_DIR=$PWD/.ccache" >> $GITHUB_ENV
echo "CCACHE_COMPRESS=true" >> $GITHUB_ENV
echo "CCACHE_COMPRESSLEVEL=6" >> $GITHUB_ENV
echo "CCACHE_MAXSIZE=400M" >> $GITHUB_ENV
