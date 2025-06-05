#!/usr/bin/env bash

# configure cmake

buildType=$1

if [[ -z $1 ]]; then
	buildType="Debug"
fi

if [[ ! -d "build/${buildType}" ]]; then
	mkdir -p build/"${buildType}"
fi

echo "using build type ${buildType}"
cmake -S . -B build/"${buildType}" -GNinja -DCMAKE_BUILD_TYPE="${buildType}" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
ln -sf build/"${buildType}"/compile_commands.json compile_commands.json
