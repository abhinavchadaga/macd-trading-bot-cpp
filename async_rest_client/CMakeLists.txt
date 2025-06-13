cmake_minimum_required(VERSION 3.28)

project(async_rest_client
        VERSION 1.0.0
        DESCRIPTION "Asynchronous C++20 REST client using Boost.Beast"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED COMPONENTS system url)
find_package(OpenSSL REQUIRED)

add_library(async_rest_client STATIC
        src/async_rest_client.cpp
        src/utils.cpp
)
target_include_directories(async_rest_client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(async_rest_client PUBLIC
        logging_utils
        Boost::system
        Boost::url
        OpenSSL::SSL
        OpenSSL::Crypto
)

enable_testing()
add_subdirectory(tests)
