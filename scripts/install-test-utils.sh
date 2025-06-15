#!/usr/bin/env bash

set -e

# Source shared logging functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/logging.sh
source "$SCRIPT_DIR/logging.sh"

# Change to test-utils directory
cd "$PROJECT_ROOT/test-utils"

# Helper functions for smart defaults
is_docker() {
	[[ -f /.dockerenv ]] || [[ -n ${DOCKER_CONTAINER:-} ]]
}

is_macos() {
	[[ "$(uname -s)" == "Darwin" ]]
}

is_writable() {
	local dir="$1"
	[[ -w $dir ]] || [[ -w "$(dirname "$dir")" ]]
}

get_shell_config_file() {
	case "${SHELL##*/}" in
	bash)
		if is_macos; then
			echo "$HOME/.bash_profile"
		else
			echo "$HOME/.bashrc"
		fi
		;;
	zsh)
		echo "$HOME/.zshrc"
		;;
	*)
		echo "$HOME/.profile"
		;;
	esac
}

ensure_path_configured() {
	local install_dir="$1"

	# Check if already in PATH
	case ":$PATH:" in
	*":$install_dir:"*)
		log_info "✓ $install_dir is already in PATH"
		return 0
		;;
	esac

	log_warning "$install_dir is not in PATH"

	local config_file
	config_file="$(get_shell_config_file)"
	local export_line="export PATH=\"$install_dir:\$PATH\""

	# Check if line already exists in config file
	if [[ -f $config_file ]] && grep -Fq "$export_line" "$config_file"; then
		log_info "PATH export already exists in $config_file"
		log_warning "You may need to restart your terminal or run: source $config_file"
		return 0
	fi

	log_info "Adding $install_dir to PATH in $config_file"

	# Create config file if it doesn't exist
	if [[ ! -f $config_file ]]; then
		touch "$config_file"
	fi

	# Add PATH export
	{
		echo ""
		echo "# Added by install-test-utils.sh"
		echo "$export_line"
	} >>"$config_file"

	log_success "Added PATH export to $config_file"
	log_warning "Please restart your terminal or run: source $config_file"
}

get_default_install_prefix() {
	if is_docker; then
		echo "/usr/local/bin"
	elif is_macos; then
		echo "$HOME/.local/bin"
	elif is_writable "/usr/local/bin"; then
		echo "/usr/local/bin"
	else
		echo "$HOME/.local/bin"
	fi
}

# Set smart default
installPrefix="$(get_default_install_prefix)"

# Parse arguments
while [[ $# -gt 0 ]]; do
	case $1 in
	--prefix | -p)
		installPrefix="$2"
		shift 2
		;;
	--help | -h)
		default_prefix="$(get_default_install_prefix)"
		echo "Usage: $0 [OPTIONS]"
		echo ""
		echo "Options:"
		echo "  -p, --prefix    Installation prefix [default: $default_prefix]"
		echo "  -h, --help      Show this help message"
		echo ""
		echo "Examples:"
		echo "  $0                        # Install to $default_prefix"
		echo "  $0 --prefix ~/bin         # Install to ~/bin"
		echo "  $0 -p /opt/bin            # Install to /opt/bin"
		exit 0
		;;
	*)
		log_error "Unknown option: $1"
		echo "Use --help for usage information"
		exit 1
		;;
	esac
done

install_py_util() {
	local script_name=$1
	local output_name="${script_name%.py}"

	local output_path="${installPrefix}/${output_name}"
	if [[ -f $output_path ]]; then
		log_info "Removing existing installation: $output_path"
		rm "$output_path"
	fi

	log_info "Building $script_name with PyInstaller..."
	# shellcheck disable=SC1091
	source venv/bin/activate || {
		log_error "Failed to activate virtual environment"
		exit 1
	}
	PYINSTALLER_ARGS=(
		--onefile --clean --specpath /tmp
		--workpath /tmp --noconfirm
		--log-level FATAL --distpath "$installPrefix"
	)
	pyinstaller "${PYINSTALLER_ARGS[@]}" "$script_name" 2>/dev/null || {
		log_error "Failed to build $script_name"
		deactivate
		exit 1
	}
	deactivate
	log_success "Installed $script_name to $output_path"
}

install_bash_script() {
	local script_name=$1
	local output_name="${script_name%.sh}"

	local output_path="${installPrefix}/${output_name}"
	if [[ -f $output_path ]]; then
		log_info "Removing existing installation: $output_path"
		rm "$output_path"
	fi

	cp "$script_name" "$output_path" || {
		log_error "Failed to copy $script_name to $output_path"
		return 1
	}
	chmod +x "$output_path" || {
		log_error "Failed to make $output_path executable"
		return 1
	}
	log_success "Installed $script_name to $output_path"
}

install_venv() {
	log_header "Setting up Python virtual environment"
	log_info "Removing existing venv..."
	rm -rf venv

	log_info "Creating new virtual environment..."
	python3 -m venv venv

	# shellcheck disable=SC1091
	source venv/bin/activate || {
		log_error "Failed to activate virtual environment"
		exit 1
	}

	log_info "Installing PyInstaller..."
	python3 -m pip install --upgrade pyinstaller >/dev/null || {
		log_error "Failed to install pyinstaller"
		deactivate
		exit 1
	}

	if [[ -f requirements.txt ]]; then
		log_info "Installing requirements from requirements.txt..."
		python3 -m pip install --upgrade -r requirements.txt >/dev/null || {
			log_error "Failed to install requirements"
			deactivate
			exit 1
		}
	else
		log_warning "No requirements.txt found, skipping dependency installation"
	fi

	deactivate
	log_success "Virtual environment setup complete"
}

log_header "Test Utils Installation"
log_info "Installation prefix: ${installPrefix}"

# Create installation directory if it doesn't exist
if [[ ! -d $installPrefix ]]; then
	log_info "Creating installation directory: $installPrefix"
	mkdir -p "$installPrefix" || {
		log_error "Failed to create installation directory: $installPrefix"
		exit 1
	}
fi

# Ensure PATH is configured for user-local installations
if [[ $installPrefix == "$HOME/.local/bin" ]]; then
	ensure_path_configured "$installPrefix"
fi

# Setup virtual environment
install_venv

# Install Python utilities
log_header "Installing Python utilities"
python_utils=$(find . -maxdepth 1 -name '*.py' | sed 's/\.\///g' | sort)

if [[ -n $python_utils ]]; then
	log_info "Found Python utilities:"
	for util in $python_utils; do
		echo "  - $util"
	done
	echo

	for script in $python_utils; do
		install_py_util "$script"
	done
else
	log_info "No Python utilities found"
fi

# Install Bash scripts
log_header "Installing Bash scripts"
bash_scripts=$(find . -maxdepth 1 -name '*.sh' | sed 's/\.\///g' | sort)

if [[ -n $bash_scripts ]]; then
	log_info "Found Bash scripts:"
	current_script="$(basename "${BASH_SOURCE[0]}")"
	for script in $bash_scripts; do
		if [[ $script != "$current_script" ]]; then
			echo "  - $script"
		fi
	done
	echo

	for script in $bash_scripts; do
		if [[ $script == "$current_script" ]]; then
			continue
		fi
		install_bash_script "$script"
	done
else
	log_info "No Bash scripts found"
fi

log_success "Installation complete!"
log_info "Utilities installed to: ${installPrefix}"
