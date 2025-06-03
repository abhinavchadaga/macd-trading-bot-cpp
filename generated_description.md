## Summary
Fixes MACD signal calculation timing issues and refactors IndicatorEngine from synchronous polling to event-driven architecture with automatic signal emission. Also adds improved build toolchain with clang+gdb configuration for better debugging support.

## Changes Made
- **MACD Signal Timing Fix**: Captures fast/slow EMA readiness state before writing new data to prevent off-by-one calculation errors in signal line computation
- **IndicatorEngine Refactor**: Removes synchronous `read()` method and implements event-driven pattern with `subscribe()` for automatic indicator updates when all indicators are ready
- **EMA Algorithm Enhancement**: Fixed initialization phase to properly use simple moving average before switching to exponential weighting
- **Build System Improvements**: Added dedicated clang+gdb toolchain configuration with proper debug symbols and architecture-specific optimizations
- **Enhanced Test Coverage**: Significantly improved indicator tests with proper readiness validation and more accurate TA-Lib comparison testing
- **VS Code Integration**: Added new build and test tasks for streamlined development workflow

## Impact
- **Improved Calculation Accuracy**: MACD signal line now calculates correctly by waiting for underlying EMAs to stabilize
- **Better Architecture**: Event-driven design eliminates polling overhead and automatically notifies subscribers when indicators update
- **Enhanced Developer Experience**: Better debugging support with clang+gdb toolchain and convenient test runner script
- **Increased Test Reliability**: More robust validation of indicator readiness states and calculation accuracy against TA-Lib reference implementation