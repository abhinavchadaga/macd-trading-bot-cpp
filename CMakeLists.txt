cmake_minimum_required(VERSION 3.28)
project(macd_trading_bot_cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost 1.88 CONFIG REQUIRED COMPONENTS system url)
find_package(nlohmann_json REQUIRED)
find_package(GTest CONFIG REQUIRED)

include_directories(include)
add_subdirectory(src)
add_subdirectory(async_rest_client)
add_subdirectory(alpaca_trade_client)
add_subdirectory(logger)

enable_testing()
add_subdirectory(tests)
