cmake_minimum_required(VERSION 3.28)

project(alpaca_trade_client
        VERSION 1.0.0
        DESCRIPTION "Alpaca Trading API client using async_rest_client"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED COMPONENTS json)

add_library(alpaca_trade_client STATIC
        src/alpaca_trade_client.cpp
        src/json_utils.cpp
)
target_include_directories(alpaca_trade_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/async_rest_client/include)
target_link_libraries(alpaca_trade_client PUBLIC
        Boost::json
        async_rest_client
)

enable_testing()
add_subdirectory(tests)
