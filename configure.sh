#!/usr/bin/env bash

# configure cmake

set -x

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

# build python scripts into executables

function has_changed() {
	local file=$1
	local basename_no_extension
	basename_no_extension=$(basename "$file" | sed 's/\.[^.]*$//')
	local timestamp_file="build/${buildType}/timestamps/${basename_no_extension}.timestamp"
	mkdir -p "build/${buildType}/timestamps"

	if [[ ! -f $file ]]; then
		return 1
	fi

	local current_mtime
	current_mtime=$(stat -c %Y "$file")

	if [ ! -f "$timestamp_file" ]; then
		echo "$current_mtime" >"$timestamp_file"
		return 0
	fi

	local saved_mtime
	saved_mtime=$(cat "$timestamp_file")

	echo "$current_mtime" >"$timestamp_file"

	[ "$current_mtime" -gt "$saved_mtime" ]
}

function build_python_test_utils() {
	local script_path=$1
	current_dir=$(pwd)
	if has_changed "$script_path"; then
		echo "Building $script_path"
		local script_dir
		script_dir=$(dirname "$script_path")

		cd "$script_dir" || return
		local script_name
		script_name=$(basename "$script_path")
		local output_name="${script_name%.py}"
		local output_dir="$current_dir/build/python-utils"
		local output_path="${output_dir}/${output_name}"
		if [[ -f $output_path ]]; then
			rm "$output_path"
		fi

		# shellcheck disable=SC1091
		source venv/bin/activate || {
			echo "Failed to activate virtual environment"
			exit 1
		}
		PYINSTALLER_ARGS=(
			--onefile --clean --specpath /tmp
			--workpath /tmp --noconfirm
			--log-level FATAL --distpath "$output_dir"
		)
		pyinstaller "${PYINSTALLER_ARGS[@]}" "$script_name" 2>/dev/null || {
			echo "Failed to build $script_name"
			deactivate
			exit 1
		}

		deactivate

		echo "Built $script_path to $output_path"
	else
		echo "Skipping $script_path, no changes detected."
	fi
	cd "$current_dir" || return
}

function install_venv() {
	echo "installing venv..."
	local dir=$1
	if [[ -d $dir/venv ]]; then
		return
	fi
	current_dir=$(pwd)
	cd "$dir" || return
	python3 -m venv venv
	# shellcheck disable=SC1091
	source venv/bin/activate || {
		echo "Failed to activate virtual environment"
		exit 1
	}
	python3 -m pip install --quiet --upgrade pyinstaller
	python3 -m pip install --quiet --upgrade -r requirements.txt || {
		echo "Failed to install requirements"
		deactivate
		exit 1
	}
	cd "$current_dir" || return
}

install_venv "test-utils"
python_utils=$(find test-utils -maxdepth 1 -name '*.py')
for script in $python_utils; do
	build_python_test_utils "$script"
done
