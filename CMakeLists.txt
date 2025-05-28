cmake_minimum_required(VERSION 3.31)
project(macd_trading_bot_cpp)

# ---------- Global compile flags ----------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# ---------- OpenSSL --------------
find_package(OpenSSL REQUIRED)

include(FetchContent)

# ---------- Boost via find_package ----------
find_package(Boost REQUIRED COMPONENTS system)

# include nlohmann json
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

# include google test
FetchContent_Declare(googletest URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz)
FetchContent_MakeAvailable(googletest)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
