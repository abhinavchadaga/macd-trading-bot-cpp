# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++23 MACD trading bot that uses the Alpaca Trading API to automatically trade based on technical indicators. The bot
follows a single-threaded, event-driven Reactor pattern inspired by Virtu Financial's architecture.

### Trading Strategy

- **Entry**: MACD line crosses above signal line while below zero, close price > EMA_200, low volatility (ATR/close < threshold)
- **Exit**: MACD crosses below signal line OR close price < EMA_200
- **Risk Management**: Stop loss at entry_price - (1.5 * ATR)
- **Position Management**: Closes positions 15 minutes before market close to stay flat overnight

## Build System

Use the provided shell scripts for building:

```bash
# Configure build (default: Debug, uses GCC toolchain)
./configure.sh [Debug|Release]

# Build project (default: Debug build, all targets)
./build.sh [Debug|Release] [target]

# Examples:
./build.sh Debug unit_tests    # Build only tests
./build.sh Release            # Build release version
```

**Manual CMake approach:**
```bash
cmake -S . -B build/Debug -GNinja -DCMAKE_TOOLCHAIN_FILE=cmake/linux-gcc.cmake
cmake --build build/Debug
```

## Testing

Run tests using:

```bash
./build.sh Debug unit_tests
./build/Debug/tests/unit_tests
```

Tests use Google Test framework and cover:
- WebSocket session management
- Alpaca market feed integration  
- Bar aggregation logic

## Dependencies

- **Boost** (system): Networking support
- **OpenSSL**: TLS/SSL for secure connections
- **nlohmann/json**: JSON parsing for API responses
- **Google Test**: Unit testing framework
- **C++23**: Modern C++ features required

Dependencies are managed via vcpkg/Conan (find_package in CMake).

## Architecture

The bot operates as a single-threaded event loop processing:

1. Real-time market data via WebSocket (1-minute candles → 5-minute aggregation via BarAggregator)
2. Technical indicator calculations (MACD, EMA_200, ATR)
3. Strategy decisions (BUY/SELL/NO_ACTION)
4. Order execution and position tracking via Alpaca API

**Key Components:**
- `AlpacaWSMarketFeed`: WebSocket client for Alpaca market data
- `BarAggregator`: Consolidates 1-minute bars into 5-minute bars
- `WebSocketSession`: Generic WebSocket session management
- `LoggingUtils`: Centralized logging using easylogging++

**Design Principle:** Components should be mockable for backtesting (Live vs Historical data streams, Live vs Mock trading clients).

## Coding Standards

1. Use Object Oriented C++23
2. Format code using clang-format's GNU preset
3. Use "_" prefix for instance variables
4. Use default initialization for instance members whenever possible
5. Prefer braced initialization over parentheses
6. Never leave inline comments - code should be self-documenting through descriptive function and variable names 
