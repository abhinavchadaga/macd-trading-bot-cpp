#!/usr/bin/env bash

./configure.sh Debug
build_cmd="./build.sh Debug unit_tests"

if ! $build_cmd; then
	echo "Build failed. Exiting."
	exit 1
fi

./build/Debug/tests/unit_tests
