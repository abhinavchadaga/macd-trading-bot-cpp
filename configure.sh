#!/usr/bin/env bash

buildType=$1

if [[ -z $1 ]]; then
	buildType="Debug"
fi

if [[ ! -d "build/${buildType}" ]]; then
	mkdir -p build/"${buildType}"
fi

echo "using build type ${buildType}"
cmake -S . -B build/"${buildType}" -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
ln -sf build/"${buildType}"/compile_commands.json compile_commands.json
