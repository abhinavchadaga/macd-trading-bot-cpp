#!/usr/bin/env bash

set -e

# Source shared logging functions
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/logging.sh
source "$SCRIPT_DIR/logging.sh"

# Change to project root to ensure relative paths work correctly
cd "$PROJECT_ROOT"

buildType="debug"
target="all"
configureFlags=""
skipConfigure=false

# Parse arguments
while [[ $# -gt 0 ]]; do
	case $1 in
	--build-type | --type | -t)
		buildType="$2"
		shift 2
		;;
	--target | -T)
		target="$2"
		shift 2
		;;
	--cmake-flags | --flags | -f)
		configureFlags="$2"
		shift 2
		;;
	--skip-configure | -s)
		skipConfigure=true
		shift
		;;
	--list-targets | -l)
		if [[ ! -d $buildDir ]]; then
			log_warning "Build directory does not exist: ${buildDir}"
			log_info "Running configure to create build files..."
			"$SCRIPT_DIR/configure.sh" --type "$buildType"
		fi

		log_header "Available Build Targets"
		log_info "Listing targets for build type: ${buildType}"
		echo
		cmake --build "$buildDir" --target help
		exit 0
		;;
	--help | -h)
		echo "Usage: $0 [OPTIONS]"
		echo ""
		echo "Options:"
		echo "  -t, --build-type, --type    Build type (Debug, Release, etc.) [default: Debug]"
		echo "  -T, --target                Build target [default: all]"
		echo "  -f, --cmake-flags, --flags  Additional CMake flags for configuration"
		echo "  -s, --skip-configure        Skip the configure step"
		echo "  -l, --list-targets          Show all available build targets"
		echo "  -h, --help                  Show this help message"
		echo ""
		echo "Examples:"
		echo "  $0                                      # Configure and build Debug"
		echo "  $0 --type Release                       # Configure and build Release"
		echo "  $0 -t Debug -T TestIndicators           # Build specific target"
		echo "  $0 --list-targets                       # Show all available targets"
		echo "  $0 --skip-configure                     # Skip configure, just build"
		echo "  $0 -t Debug -f \"-DCUSTOM_FLAG=ON\"      # Configure with additional flags"
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

if [[ $skipConfigure == false ]]; then
	log_header "Configuration Phase"
	if [[ -n $configureFlags ]]; then
		log_info "Running configure with additional flags: ${configureFlags}"
		"$SCRIPT_DIR/configure.sh" --type "$buildType" --cmake-flags "$configureFlags"
	else
		log_info "Running configure for build type: ${buildType}"
		"$SCRIPT_DIR/configure.sh" --type "$buildType"
	fi
else
	log_warning "Skipping configuration step"
fi

log_header "Build Phase: ${buildType}"
log_info "Building target: ${target}"
log_info "Build directory: ${buildDir}"

if [[ ! -d $buildDir ]]; then
	log_error "Build directory does not exist: ${buildDir}"
	log_error "Run without --skip-configure to create it"
	exit 1
fi

cmake --build "$buildDir" --target "$target"

log_success "Build complete!"
echo
