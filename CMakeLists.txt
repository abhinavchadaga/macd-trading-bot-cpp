cmake_minimum_required(VERSION 3.31)
project(macd_trading_bot_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(third-party)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
