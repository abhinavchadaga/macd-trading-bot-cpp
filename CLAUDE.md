# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A C++20 MACD trading bot that uses the Alpaca Trading API to automatically trade based on technical indicators. The bot
follows a single-threaded, event-driven Reactor pattern inspired by Virtu Financial's architecture.

### Trading Strategy

- **Entry**: MACD crossover above signal line when below zero, close price > EMA_200, low volatility (ATR check)
- **Exit**: MACD crosses below signal line OR close price < EMA_200
- **Risk Management**: Stop loss at entry_price - (1.5 * ATR)

## Build System

This project uses CMake with FetchContent to manage dependencies:

```bash
# Configure and build
mkdir build && cd build
cmake ..
make

# Run from build directory
./src/main                    # Main trading bot
./src/examples/daytime_server # Example WebSocket server
```

## Dependencies

- **Boost** (asio, beast, system): Networking and WebSocket support
- **nlohmann/json**: JSON parsing for API responses
- **C++20**: Modern C++ features required

All dependencies are automatically fetched via CMake FetchContent.

## Architecture

The bot operates as a single-threaded event loop processing:

1. Real-time market data via WebSocket (1-minute candles → 5-minute aggregation)
2. Technical indicator calculations (MACD, EMA_200, ATR)
3. Strategy decisions (BUY/SELL/NO_ACTION)
4. Order execution and position tracking via Alpaca API

Key design principle: Components should be mockable for backtesting (Live vs Historical data streams, Live vs Mock
trading clients).

## File Structure

- `src/': impleme
- `tests/`: Test directory (currently empty)

## Testing

Tests are built as a separate target:

```bash
# From build directory
make tests  # Build tests (excluded from default build)
# No test runner specified yet - check project status
```

## Coding Standards:

1. Use Object Oriented C++20
2. Format your code using clang-format's GNU preset
3. Use "_" as a prefix for instance variables
4. Use default initialization for instance members whenever possible
5. Prefer braced initialization over parentheses
6. Never leave inline comments - the code should be "self-documenting" through descriptive function and variable names 
