# CMake toolchain file for Linux with GCC

set(CMAKE_SYSTEM_NAME Linux)

# Auto-detect processor architecture
execute_process(
    COMMAND uname -m
    OUTPUT_VARIABLE DETECTED_ARCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(DETECTED_ARCH STREQUAL "aarch64" OR DETECTED_ARCH STREQUAL "arm64")
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
elseif(DETECTED_ARCH STREQUAL "x86_64")
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
else()
    message(WARNING "Unknown architecture: ${DETECTED_ARCH}, defaulting to x86_64")
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
endif()

message(STATUS "detected arch -> ${DETECTED_ARCH}")

# Specify the cross compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Compiler flags
set(CMAKE_CXX_FLAGS_INIT "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
