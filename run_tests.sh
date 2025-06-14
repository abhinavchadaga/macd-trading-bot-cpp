#!/usr/bin/env bash

set -e

build_type=${1:-Release}
dep_install_dir=${2:-/usr/local/bin}

echo "Installing dependencies to $dep_install_dir"

echo ""
echo "--------------------------------"
echo "Building tests"
echo "--------------------------------"
echo ""

./test-utils/install.sh "$dep_install_dir"
./build.sh "$build_type"

echo ""
echo "--------------------------------"
echo "Running all tests..."
echo "--------------------------------"
echo ""

cd "build/$build_type" && ctest build/"$build_type" --progress --output-on-failure
