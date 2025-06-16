find_package(GTest REQUIRED)
include(GoogleTest)

FILE(GLOB TEST_SRC "Test*.cpp")

add_executable(async_rest_client_tests
    test_main.cpp
    ${TEST_SRC}
)

target_link_libraries(async_rest_client_tests
    async_rest_client
    GTest::gtest
    my_logger
)

gtest_discover_tests(async_rest_client_tests)
