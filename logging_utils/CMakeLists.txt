cmake_minimum_required(VERSION 3.28)

project(logging_utils
    VERSION 1.0.0
    DESCRIPTION "Logging utilities using easylogging++"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logging_utils STATIC
    src/LoggingUtils.cpp
)

target_include_directories(logging_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third-party
)

target_link_libraries(logging_utils PUBLIC
    easylogging
)
