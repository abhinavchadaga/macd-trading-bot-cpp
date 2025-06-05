#!/usr/bin/env bash

cd "$(dirname "$0")" || exit 1

INSTALL_PREFIX="/usr/local/bin"

function install_py_util() {
	local script_name=$1
	local output_name="${script_name%.py}"

	local output_path="${INSTALL_PREFIX}/${output_name}"
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
		--log-level FATAL --distpath "$INSTALL_PREFIX"
	)
	pyinstaller "${PYINSTALLER_ARGS[@]}" "$script_name" 2>/dev/null || {
		echo "Failed to build $script_name"
		deactivate
		exit 1
	}
	deactivate
	echo "Installed $script_name to $output_path"
}

function install_bash_script() {
	local script_name=$1
	local output_name="${script_name%.sh}"

	local output_path="${INSTALL_PREFIX}/${output_name}"
	if [[ -f $output_path ]]; then
		rm "$output_path"
	fi

	cp "$script_name" "$output_path" || {
		echo "Failed to copy $script_name to $output_path"
		return 1
	}
	chmod +x "$output_path" || {
		echo "Failed to make $output_path executable"
		return 1
	}
	echo "Installed $script_name to $output_path"
}

function install_venv() {
	echo "installing venv..."
	rm -rf venv
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
python_utils=$(find . -maxdepth 1 -name '*.py' | sed 's/\.\///g' | sort)

for script in $python_utils; do
	echo "processing $script..."
	install_py_util "$script"
done

bash_scripts=$(find . -maxdepth 1 -name '*.sh' | sed 's/\.\///g' | sort)

for script in $bash_scripts; do
	if [[ $script == "install.sh" ]]; then
		continue
	fi
	echo "processing $script..."
	install_bash_script "$script"
done
