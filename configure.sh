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
	if has_changed "$script_path"; then
		echo "Building $script_path"
		local script_dir
		script_dir=$(dirname "$script_path")
		local script_name
		script_name=$(basename "$script_path")
		local output_name="${script_name%.py}"
		local output_dir="build/python-utils"
		local output_path="${output_dir}/${output_name}"
		if [[ -f $output_path ]]; then
			rm "$output_path"
		fi

		# shellcheck disable=SC1091
		source "$script_dir/venv/bin/activate"
		python3 -m pip install pyinstaller --quiet --upgrade

		build_cmd="pyinstaller --onefile --clean --specpath /tmp --workpath /tmp --noconfirm --log-level FATAL --distpath $output_dir $script_path 2>/dev/null"
		if ! eval "$build_cmd"; then
			deactivate
			echo "Failed to build $script_path"
			exit 1
		fi

		deactivate

		echo "Built $script_path to $output_path"
	else
		echo "Skipping $script_path, no changes detected."
	fi
}

python_utils=$(find test-utils -maxdepth 1 -name '*.py')

for script in $python_utils; do
	build_python_test_utils "$script"
done
