#!/usr/bin/env bash

cd "$(dirname "$0")" || exit 1
output_dir="../build/python-utils"

function has_changed() {
	local file=$1
	local basename_no_extension
	basename_no_extension=$(basename "$file" | sed 's/\.[^.]*$//')
	local timestamp_file="/tmp/${basename_no_extension}.timestamp"
	if [ ! -f "$output_dir/$basename_no_extension" ]; then
		rm -f timestamp_file
		return 0
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
	local script_name=$1
	if has_changed "$script_name"; then
		echo "Building $script_name"
		local output_name="${script_name%.py}"
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

		echo "Built $script_name to $output_path"
	else
		echo "Skipping $script_name, no changes detected."
	fi
}

function install_venv() {
	echo "installing venv..."
	if [[ -d "venv" ]]; then
		return
	fi
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
}

install_venv
python_utils=$(find . -maxdepth 1 -name '*.py')
for script in $python_utils; do
	echo processing "$script"...
	build_python_test_utils "$script"
done
