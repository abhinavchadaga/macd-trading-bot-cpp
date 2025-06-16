#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/logging.sh
source "$SCRIPT_DIR/logging.sh"

cd "$PROJECT_ROOT"

build_project() {
	local build_type="$1"
	local configure_flags="$2"

	if [[ -n $configure_flags ]]; then
		"$SCRIPT_DIR/build.sh" --build-type "$build_type" --cmake-flags "$configure_flags"
	else
		"$SCRIPT_DIR/build.sh" --build-type "$build_type"
	fi
}

list_available_tests() {
	local build_dir="$1"
	local build_type="$2"
	local configure_flags="$3"

	if [[ ! -d $build_dir ]]; then
		log_warning "Build directory does not exist: $build_dir"
		echo
		read -p "Would you like to build the project first? (y/N): " -n 1 -r
		echo
		if [[ $REPLY =~ ^[Yy]$ ]]; then
			log_info "Building project to generate tests..."
			if [[ -n $configure_flags ]]; then
				log_info "Using CMake flags: $configure_flags"
			fi
			echo
			# Build with the same configure flags as normal operation
			build_project "$build_type" "$configure_flags"
			echo
			log_info "Build complete, listing tests..."
		else
			log_info "Cancelled. Run the build first or check your build type"
			return 1
		fi
	fi

	log_header "Available Tests"
	log_info "Discovering tests in build directory: $build_dir"

	if ! (cd "$build_dir" && ctest --show-only=json-v1 2>/dev/null | jq -e '.tests | length > 0' >/dev/null 2>&1); then
		log_warning "No tests found in $build_dir"
		log_info "Make sure you have built the project with tests enabled"
		return 1
	fi

	echo
	log_info "Available tests:"
	(cd "$build_dir" && ctest --show-only=json-v1 2>/dev/null | jq -r '.tests[].name' | sort | while read -r test_name; do
		echo "  - $test_name"
	done)

	echo
	local test_count
	test_count=$(cd "$build_dir" && ctest --show-only=json-v1 2>/dev/null | jq '.tests | length')
	log_info "Found $test_count tests total"
}

run_tests() {
	local build_dir="$1"
	local test_pattern="$2"
	local verbose="$3"

	if [[ ! -d $build_dir ]]; then
		log_error "Build directory does not exist: $build_dir"
		log_error "This is an internal error - build should have been completed before calling run_tests"
		return 1
	fi

	cd "$build_dir"

	local ctest_args=("--progress" "--output-on-failure")

	if [[ -n $test_pattern ]]; then
		log_header "Running Tests: $test_pattern"
		ctest_args+=("-R" "$test_pattern")
	else
		log_header "Running All Tests"
	fi

	if [[ $verbose == "true" ]]; then
		ctest_args+=("-V")
	fi

	log_info "Running tests"
	log_info "Executing: ctest ${ctest_args[*]}"
	echo

	if ctest "${ctest_args[@]}"; then
		echo
		log_success "All tests completed successfully!"
	else
		echo
		log_error "Some tests failed!"
		return 1
	fi
}

buildType="debug"
testPattern=""
installUtils=false
utilsPrefix=""
verbose=false
listTests=false

configureFlags=""

while [[ $# -gt 0 ]]; do
	case $1 in
	--build-type | --type | -t)
		buildType="$2"
		shift 2
		;;
	--test | -T)
		testPattern="$2"
		shift 2
		;;
	--install-utils)
		installUtils=true
		shift
		;;
	--no-install-utils)
		installUtils=false
		shift
		;;
	--prefix | -p)
		utilsPrefix="$2"
		shift 2
		;;
	--verbose | -v)
		verbose=true
		shift
		;;
	--list-tests | -l)
		listTests=true
		shift
		;;
	--configure-flags)
		configureFlags="$2"
		shift 2
		;;
	--help | -h)
		echo "Usage: $0 [OPTIONS]"
		echo ""
		echo "Options:"
		echo "  -t, --build-type, --type    Build type [default: debug]"
		echo "  -T, --test                  Run specific test pattern (regex)"
		echo "  -l, --list-tests            Show all available tests"
		echo "  --install-utils             Install test-utils"
		echo "  --no-install-utils          Skip test-utils installation [default]"
		echo "  -p, --prefix                Test-utils installation prefix [smart default]"
		echo "  -v, --verbose               Verbose test output"
		echo "  --configure-flags           Additional CMake flags for configure script"
		echo "  -h, --help                  Show this help message"
		echo ""
		echo "Examples:"
		echo "  $0                              # Install utils, build Release, run all tests"
		echo "  $0 --build-type Debug          # Debug build with all tests"
		echo "  $0 --test TestIndicators       # Run tests matching 'TestIndicators'"
		echo "  $0 --test \"Test.*Utils\"        # Run tests matching regex pattern"
		echo "  $0 --list-tests                # Show available tests"
		echo "  $0 --no-install-utils -v        # Skip utils, run with verbose output"
		echo "  $0 --configure-flags \"-DCUSTOM_FLAG=ON\"  # Pass custom CMake flags"
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

if [[ $listTests == true ]]; then
	list_available_tests "$buildDir" "$buildType" "$configureFlags"
	exit 0
fi

log_header "Test Execution Setup"
log_info "Build type: $buildType"
log_info "Build directory: $buildDir"
if [[ -n $testPattern ]]; then
	log_info "Test pattern: $testPattern"
else
	log_info "Running all tests"
fi
log_info "Install test-utils: $installUtils"
if [[ $verbose == true ]]; then
	log_info "Verbose output: enabled"
fi

if [[ $installUtils == true ]]; then
	log_header "Installing Test Utilities"
	if [[ -n $utilsPrefix ]]; then
		log_info "Installing test-utils with custom prefix: $utilsPrefix"
		"$SCRIPT_DIR/install-test-utils.sh" --prefix "$utilsPrefix"
	else
		log_info "Installing test-utils with smart defaults"
		"$SCRIPT_DIR/install-test-utils.sh"
	fi
else
	log_info "Skipping test-utils installation"
fi

log_header "Building Project"
log_info "Building project with all tests enabled"
if [[ -n $configureFlags ]]; then
	log_info "Using custom CMake flags: $configureFlags"
fi
build_project "$buildType" "$configureFlags"

run_tests "$buildDir" "$testPattern" "$verbose"

log_success "Test execution complete!"
