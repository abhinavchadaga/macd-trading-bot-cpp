#!/usr/bin/env bash

cd "$(dirname "$0")" || exit 1

INSTALL_PREFIX="$1"
if [[ -z $INSTALL_PREFIX ]]; then
	INSTALL_PREFIX="/usr/local/bin"
fi

echo "Installing to $INSTALL_PREFIX"
echo ""

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
	echo "installed $script_name to $output_path"
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
	echo "--------------------------------"
	echo "Setting up new venv..."
	echo "--------------------------------"
	echo ""

	rm -rf venv
	python3 -m venv venv
	# shellcheck disable=SC1091
	source venv/bin/activate || {
		echo "Failed to activate virtual environment"
		exit 1
	}
	python3 -m pip install --upgrade pyinstaller || {
		echo "Failed to install pyinstaller"
		deactivate
		exit 1
	}
	python3 -m pip install --upgrade -r requirements.txt || {
		echo "Failed to install requirements"
		deactivate
		exit 1
	}
}

install_venv
echo ""

echo "--------------------------------"
echo "Installing python utilities..."
echo "--------------------------------"
echo ""

python_utils=$(find . -maxdepth 1 -name '*.py' | sed 's/\.\///g' | sort)

echo "found python utils: "
echo "${python_utils[@]}"
echo ""

for script in $python_utils; do
	echo "compiling and installing $script..."
	install_py_util "$script"
done

echo ""

echo "--------------------------------"
echo "Installing bash scripts..."
echo "--------------------------------"
echo ""

bash_scripts=$(find . -maxdepth 1 -name '*.sh' | sed 's/\.\///g' | sort)

echo "found bash scripts: "
echo "${bash_scripts[@]}"
echo ""

for script in $bash_scripts; do
	if [[ $script == "install.sh" ]]; then
		continue
	fi
	echo "installing $script..."
	install_bash_script "$script"
done

echo ""

echo "--------------------------------"
echo "Install Complete!!!"
echo "--------------------------------"
