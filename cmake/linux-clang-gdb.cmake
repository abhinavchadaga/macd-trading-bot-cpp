# Set system name
set(CMAKE_SYSTEM_NAME Linux)

# Detect architecture
execute_process(COMMAND uname -m OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})

message(STATUS "Detected architecture: ${CMAKE_SYSTEM_PROCESSOR}")

# Specify Clang compiler
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

# Prevent stripping of debug symbols
set(CMAKE_STRIP "")

# Set base compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -fPIC -march=native -mtune=native")

# Debug configuration - GDB-compatible flags for Clang
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -gdwarf-4 -fno-omit-frame-pointer -O0" CACHE STRING "Debug flags")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-g" CACHE STRING "Debug linker flags")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "-g" CACHE STRING "Debug shared linker flags")

# Architecture specific flags
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=apple-m1")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable LTO for release builds
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
if(supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

# Set position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
