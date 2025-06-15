# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

### Configuration and Building

```bash
# Configure project (default: Debug)
./configure.sh [Debug|Release]

# Configure and Build project (default: Debug, all targets)
./build.sh [Debug|Release] [target]
```

### Testing
```bash
# Run all tests (builds dependencies and runs ctest)
./run_tests.sh [Release] [/usr/local/bin]

# Run individual test executables (after building)
cd build/Debug && ./TestIndicators
cd build/Debug && ./TestBarAggregatorIntegration
```

### Other Development Tools
```bash
# Setup git hooks
./setup-hooks.sh

# Start WebSocket echo server for testing
./test-utils/start_websocat_echo.sh

# Install Python test utilities
./test-utils/install.sh
```

## Architecture Overview

This is a C++20 MACD trading bot implementing a **Reactor pattern** with single-threaded, event-driven architecture. The bot follows OOD principles and uses modern C++20 features including concepts, ranges, and coroutines.

### Core Components

1. **Market Data Feed** (`AlpacaWSMarketFeed`) - WebSocket-based real-time market data from Alpaca, processes 1-minute bars and consolidates to 5-minute bars

2. **Bar System** (`Bar.hpp`, `BarAggregator.hpp`) - Template-based type-safe bar system supporting different timeframes with aggregation from 1-minute to 5-minute bars

3. **Indicator Engine** (`IndicatorEngine.hpp`) - Template-based engine processing multiple indicators (MACD, EMA, ATR) with signal-based notifications

4. **Async REST Client** (`async_rest_client/`) - Custom C++20 coroutine-based REST client library for trading API calls

### Build System Architecture

- **Static Library**: `macd-trading-bot` contains core trading logic
- **Test Structure**: Each test file becomes a separate executable using `--whole-archive` linking
- **Dependencies**: Boost 1.88, OpenSSL, nlohmann_json, GoogleTest, TA-Lib
- **CMake**: Uses Ninja generator with `compile_commands.json` for IDE support

### Trading Strategy

**Entry Conditions** (all must be met):
- `close > EMA_200`
- MACD line crosses above signal line
- MACD line below zero  
- ATR/close < threshold

**Exit Conditions** (either):
- `close < EMA_200`
- MACD line crosses below signal line

**Risk Management**: Stop loss at `entry_price - (1.5 * ATR)`

## Important Code Patterns

### Template-Based Design
The codebase extensively uses C++20 templates for type safety:
- `Bar<TimeFrame>` for different timeframe bars
- `IndicatorEngine<IndicatorTypes...>` for processing multiple indicators
- Template metaprogramming for compile-time type checking

### Signal-Based Event Handling
Uses Boost.Signals2 for decoupled event handling between components like market data feed and indicator processing.

### Modular Testing Architecture
Each test file is a separate executable to avoid symbol conflicts. Tests use static linking with `--whole-archive` to ensure all symbols are available.

## Development Notes

- The project follows C++ Core Guidelines and uses modern C++20 idioms
- All classes follow the header/implementation separation pattern (except templates)
- WebSocket connections use Boost.Beast for HTTP/WebSocket communication
- The `async_rest_client` library demonstrates advanced C++20 coroutine usage
- Python utilities in `test-utils/` provide historical data testing and WebSocket debugging tools
