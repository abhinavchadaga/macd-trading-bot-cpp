#!/usr/bin/env bash

set -e

build_type=${1:-Release}
dep_install_dir=${2:-/usr/local/bin}

echo "Installing dependencies to $dep_install_dir"

echo ""
echo "--------------------------------"
echo "Running unit_tests"
echo "--------------------------------"
echo ""

./test-utils/install.sh "$dep_install_dir"
./build.sh "$build_type" unit_tests

echo ""
echo "--------------------------------"
echo "Running unit_tests..."
echo "--------------------------------"
echo ""

./build/"$build_type"/tests/unit_tests
