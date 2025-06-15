#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
# shellcheck source=scripts/logging.sh
source "$SCRIPT_DIR/logging.sh"

cd "$PROJECT_ROOT"

get_parallel_jobs() {
	if command -v nproc >/dev/null 2>&1; then
		nproc
	elif [[ -r /proc/cpuinfo ]]; then
		grep -c ^processor /proc/cpuinfo
	else
		echo "4" # fallback
	fi
}

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
	local parallel_jobs="$3"
	local verbose="$4"

	# Assume build directory exists - caller is responsible for ensuring this
	if [[ ! -d $build_dir ]]; then
		log_error "Build directory does not exist: $build_dir"
		log_error "This is an internal error - build should have been completed before calling run_tests"
		return 1
	fi

	cd "$build_dir"

	# Build ctest command
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

	if [[ -n $parallel_jobs && $parallel_jobs -gt 1 ]]; then
		ctest_args+=("-j" "$parallel_jobs")
		log_info "Running tests with $parallel_jobs parallel jobs"
	fi

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

# Default values
buildType="debug"
testPattern=""
installUtils=false
utilsPrefix=""
parallelJobs=""
verbose=false
listTests=false

# Parameters to pass to intermediate scripts
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
	--parallel | -j)
		parallelJobs="$2"
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
		echo "  -j, --parallel              Number of parallel test jobs [default: auto-detect]"
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
		echo "  $0 --no-install-utils -j 4     # Skip utils, run with 4 parallel jobs"
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

# Handle list-tests option
if [[ $listTests == true ]]; then
	list_available_tests "$buildDir" "$buildType" "$configureFlags"
	exit 0
fi

if [[ -z $parallelJobs ]]; then
	parallelJobs=$(get_parallel_jobs)
fi

log_header "Test Execution Setup"
log_info "Build type: $buildType"
log_info "Build directory: $buildDir"
if [[ -n $testPattern ]]; then
	log_info "Test pattern: $testPattern"
else
	log_info "Running all tests"
fi
log_info "Parallel jobs: $parallelJobs"
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

# Ensure build directory exists
if [[ ! -d $buildDir ]]; then
	log_header "Building Project"
	log_info "Build directory does not exist, building with all tests enabled"
	if [[ -n $configureFlags ]]; then
		log_info "Using custom CMake flags: $configureFlags"
	fi
	build_project "$buildType" "$configureFlags"
else
	log_info "Build directory exists: $buildDir"
fi

# Run tests
run_tests "$buildDir" "$testPattern" "$parallelJobs" "$verbose"

log_success "Test execution complete!"
