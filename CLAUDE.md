# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++20 MACD trading bot that uses the Alpaca Trading API to automatically trade based on technical indicators. The bot follows a single-threaded, event-driven Reactor pattern inspired by Virtu Financial's architecture.

### Trading Strategy

- **Entry**: MACD crossover above signal line when below zero, close price > EMA_200, low volatility (ATR check)
- **Exit**: MACD crosses below signal line OR close price < EMA_200
- **Risk Management**: Stop loss at entry_price - (1.5 * ATR)

## Build System

This project uses CMake with FetchContent to manage dependencies. The build directory is `cmake-build-debug` (CLion convention).

```bash
# Configure and build (from project root)
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B cmake-build-debug
cmake --build cmake-build-debug

# Run tests
cmake --build cmake-build-debug --target unit_tests
./cmake-build-debug/tests/unit_tests

# Run specific test
./cmake-build-debug/tests/unit_tests --gtest_filter="WebSocketSessionTest.*"
```

## Dependencies

All dependencies are automatically fetched via CMake FetchContent:
- **Boost** (asio, beast, system): Networking and WebSocket support
- **nlohmann/json**: JSON parsing for API responses  
- **GoogleTest**: Testing framework
- **OpenSSL**: Required for SSL/TLS connections
- **C++20**: Modern C++ features required

## Architecture

The bot operates as a single-threaded event loop processing:

1. Real-time market data via WebSocket (1-minute candles → 5-minute aggregation)
2. Technical indicator calculations (MACD, EMA_200, ATR)  
3. Strategy decisions (BUY/SELL/NO_ACTION)
4. Order execution and position tracking via Alpaca API

### Key Components

- **web_socket_session**: SSL WebSocket client with automatic reconnection, heartbeat, and error handling
- **WebSocketSessionTest**: Automated test that spawns websocat echo server for integration testing

Key design principle: Components should be mockable for backtesting (Live vs Historical data streams, Live vs Mock trading clients).

## Testing

WebSocket tests are fully automated - no manual setup required:

```bash
# All tests
cmake --build cmake-build-debug --target unit_tests && ./cmake-build-debug/tests/unit_tests

# WebSocket tests specifically  
./cmake-build-debug/tests/unit_tests --gtest_filter="WebSocketSessionTest.*"
```

The WebSocket test automatically:
1. Downloads and installs websocat if needed
2. Generates SSL certificates
3. Starts SSL WebSocket echo server on port 9001
4. Runs connection and echo tests
5. Cleans up server on completion

## Development Tools

### Pre-commit Hooks

Local pre-commit hooks enforce code quality with clang-format (GNU style), clang-tidy, and Conventional Commits:

```bash
# Setup (run once)
./setup-hooks.sh

# Easy commit with auto-fixes
./git-commit-with-hooks.sh "feat: your commit message"

# Manual hook run
pre-commit run --all-files

# Traditional git workflow (hooks auto-fix formatting)
git add .
git commit -m "your message"
# If hooks make changes, re-run:
git add . && git commit --amend --no-edit
```

### Code Quality

- **Formatting**: Uses clang-format with GNU style (auto-fixed by hooks)
- **Linting**: clang-tidy with CLion defaults (validates during commit)
- **Commit Messages**: Conventional Commits format enforced locally

Examples:
- `feat: add MACD trading strategy`
- `fix(websocket): handle connection timeouts`
- `test: add WebSocket echo integration test`

## Coding Standards

1. Use Object Oriented C++20
2. Format code using clang-format's GNU preset  
3. Use "_" as prefix for instance variables
4. Use default initialization for instance members whenever possible
5. Prefer braced initialization over parentheses
6. Never leave inline comments - code should be self-documenting through descriptive function and variable names
