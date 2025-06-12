# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Related and necessary file

- See @README.md for information about this project including a general overview, strategy specifications, and more.
- See @CMakeLists.txt for information about how this project is built. Modify this file to add new dependencies to the project
- See @configure.sh for a script that runs the cmake configure step
- See @build.sh for a script that can configure and build the project
- See @run_tests.sh for a script that builds test dependencies and runs ALL tests. Right now, this script does not support running specific tests.

## Build System

CMake 3.28+ with C++20 standard. Uses Ninja generator for fast builds.

### CMake Configure + Build Scripts

```bash
# Configure and build
./configure.sh [Debug|Release]     # Configure build (Debug default)
./build.sh [Debug|Release] [target] # Build all or specific target

# Testing
./run_tests.sh [Release|Debug] [install_dir] # Run all tests

# Examples
./build.sh Debug              # Build all in Debug mode
./build.sh Release TestBar    # Build specific target in Release
./run_tests.sh Release        # Run all tests in Release mode
```

## Architecture

Single-threaded, event-driven **Proactor pattern** using Boost.Asio coroutines. Multi-library structure:

1. **`macd-trading-bot`** - Main trading logic
2. **`async_rest_client`** - Independent REST client library
3. **`logging_utils`** - Centralized logging utilities

### Core Data Flow

```text
AlpacaWSMarketFeed -> BarAggregator -> IndicatorEngine -> Strategy -> TradeEngine
   (WebSocket)        (1min->5min)     (MACD/EMA/ATR)
```

Uses **Boost.Signals2** for decoupled component communication.

## Key Dependencies

- **Boost** (system, url) - Networking, coroutines, signals
- **OpenSSL** - TLS/SSL support
- **nlohmann_json** - JSON parsing
- **GoogleTest** - Unit testing
- **easylogging++** - Logging (third-party)
- **TA-Lib** - Technical analysis validation

## Trading Strategy

MACD crossover strategy with entry conditions:

1. `close_price > EMA_200`
2. MACD line crosses above signal line
3. MACD line below zero
4. `ATR/close_price < ATR_threshold`

Stop-loss at `entry_price - (1.5 * atr)`.

## Development Standards

### C++ Standards

- Use modern C++20 constructs and C++ Core Guidelines
- Braced initialization `{}` preferred over `=` or `()`
- Self-documenting code through proper naming - **no inline comments**
- snake_case for variables/functions/classes, `_` prefix for instance variables
- Class organization: static members → public → protected → private (functions then members)

### Logging Standards

- Use easylogging++ with `LoggingUtils.hpp` macros
- Format: `LOG_INFO(ClassName, function_name)`
- Call `configure_logging()`, then `CLASS_LOGGER(class_name)` in the constructor of a class
- Log entry/exit points for critical operations and all error conditions

### Testing Patterns

- Each test file = separate executable (no single unit_tests target)
- Pattern: `Test<ComponentName>.cpp`
- Build incrementally: `./build.sh Debug <test_name>`
- To run a specific test, right now, run the specific test executable directly and specify and necessary gtest filters.
- Mock external dependencies, test in isolation

### Build Workflow

- Always use `./configure.sh` and `./build.sh` scripts
- Build smallest target possible during development
- Incremental building: `./build.sh Debug <specific_target>`
- Test compilation before proceeding to next feature

### Project Specific Coding Standards

- Headers in `include/` with matching implementation in `src/`
- Use `#pragma once` for header guards
- Group includes: system headers → third-party libraries → project headers
- Use forward declarations to reduce compilation dependencies
- Template implementations should be included in header files
- Use `std::enable_shared_from_this` for classes needing shared_ptr to themselves
- Private constructors with static `create()` factory methods for shared resource classes
- Configuration structs as nested types within the class they configure
- Follow Proactor pattern for event-driven, single-threaded asynchronous operations
- Components should be mockable for testing and backtesting
- Use dependency injection through constructor parameters
- When giving edits, format your code according to the modified GNU clang-format preset as specified in @.clang-format

### Claude Code Rules

- Only implement code changes when I explictly give permission
- Opt to work in small, testable chunks, and make smaller tool calls rather than huge diffs
- Unless you've been explicitly instructed to work in a loop and do compilation and test running, let me run these steps manually
- Always use the provided bash scripts for configuring (`configure.sh`) and building code (`build.sh`)

### Memories

- Stop using the cd command. I have it aliased to zoxide which you for some reason cannot access or see
