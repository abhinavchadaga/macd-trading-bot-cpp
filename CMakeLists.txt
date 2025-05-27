cmake_minimum_required(VERSION 3.31)
project(macd_trading_bot_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)
find_package(nlohmann_json REQUIRED)
find_package(GTest CONFIG REQUIRED)

# include easylogging++
set(build_static_lib ON CACHE BOOL "" FORCE)
FetchContent_Declare(
        easyloggingpp
        GIT_REPOSITORY https://github.com/abumq/easyloggingpp.git
        GIT_TAG v9.97.1
)
FetchContent_MakeAvailable(easyloggingpp)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
