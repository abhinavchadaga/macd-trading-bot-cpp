cmake_minimum_required(VERSION 3.28)

project(async_rest_client
    VERSION 1.0.0
    DESCRIPTION "Asynchronous C++20 REST client using Boost.Beast"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)

add_library(async_rest_client STATIC
    src/async_rest_client.cpp
    src/typed_task.cpp
)

target_include_directories(async_rest_client PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(async_rest_client PUBLIC
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_compile_features(async_rest_client PUBLIC cxx_std_20)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
