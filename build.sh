#!/usr/bin/env bash

set -e

buildType=${1:-Debug}
buildDir="build/${buildType}"
target=${2:-all}

./configure.sh "$buildType"

echo "--------------------------------"
echo "CMake Build: ${buildType}"
echo "--------------------------------"

cmake --build "$buildDir" --target "$target"

echo ""
echo "--------------------------------"
echo "Build Complete!!!"
echo "--------------------------------"
