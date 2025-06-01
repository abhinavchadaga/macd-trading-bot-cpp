#!/usr/bin/env bash

buildType="$1"
target="$2"

if [[ -z $buildType ]]; then
	buildType="Debug"
fi

if [[ -z $target ]]; then
	target="all"
fi

./configure.sh "$buildType"
cmake --build build/"$buildType" --target "$target"
