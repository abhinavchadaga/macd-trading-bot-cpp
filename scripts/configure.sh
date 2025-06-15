#!/usr/bin/env bash

set -e

# Source shared logging functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/logging.sh
source "$SCRIPT_DIR/logging.sh"

# Change to project root to ensure relative paths work correctly
cd "$PROJECT_ROOT"

# Default values
buildType="debug"
additionalFlags=""

while [[ $# -gt 0 ]]; do
	case $1 in
	--build-type | --type | -t)
		buildType="$2"
		shift 2
		;;
	--cmake-flags | --flags | -f)
		additionalFlags="$2"
		shift 2
		;;
	--help | -h)
		echo "Usage: $0 [OPTIONS]"
		echo ""
		echo "Options:"
		echo "  -t, --build-type, --type    Build type (Debug, Release, etc.) [default: debug]"
		echo "  -f, --cmake-flags, --flags  Additional CMake flags"
		echo "  -h, --help                  Show this help message"
		echo ""
		echo "Examples:"
		echo "  $0                                    # Configure with Debug build type"
		echo "  $0 --type Release                     # Configure with Release build type"
		echo "  $0 -t Debug -f \"-DCUSTOM_FLAG=ON\"    # Configure with additional CMake flags"
		exit 0
		;;
	*)
		log_error "Unknown option: $1"
		echo "Use --help for usage information"
		exit 1
		;;
	esac
done

buildDir="build/${buildType}"

log_header "CMake Configure: ${buildType}"
log_info "Creating build directory: ${buildDir}"
mkdir -p "${buildDir}"

log_info "Running CMake configuration..."
if [[ -n $additionalFlags ]]; then
	log_info "Additional CMake flags: ${additionalFlags}"
	cmake -S . -B "$buildDir" -GNinja -DCMAKE_BUILD_TYPE="$buildType" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON "$additionalFlags"
else
	cmake -S . -B "$buildDir" -GNinja -DCMAKE_BUILD_TYPE="$buildType" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

log_header "Linking compile_commands.json"
log_info "Creating symlink: build/$buildType/compile_commands.json -> compile_commands.json"
ln -sf build/"$buildType"/compile_commands.json compile_commands.json

log_success "CMake configuration complete!"
echo
