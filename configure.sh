#!/usr/bin/env bash

set -e

buildType=${1:-Debug}
buildDir="build/${buildType}"

if [[ ! -d ${buildDir} ]]; then
	mkdir -p "${buildDir}"
fi

echo "--------------------------------"
echo "CMake Configure: ${buildType}"
echo "--------------------------------"
echo ""

cmake -S . -B "$buildDir" -GNinja -DCMAKE_BUILD_TYPE="$buildType" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo ""
echo "--------------------------------"
echo "Linking compile_commands.json"
echo "--------------------------------"
echo ""

echo "Linking: build/$buildType/compile_commands.json -> compile_commands.json"
ln -sf build/"$buildType"/compile_commands.json compile_commands.json

echo ""
echo "--------------------------------"
echo "CMake Configure Complete!!!"
echo "--------------------------------"
