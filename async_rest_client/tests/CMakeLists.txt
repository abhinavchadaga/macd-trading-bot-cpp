find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(TestAsyncRestClient
    TestAsyncRestClient.cpp
)

target_link_libraries(TestAsyncRestClient
    logging_utils
    async_rest_client
    Boost::system
    Boost::url
    GTest::gtest_main
    GTest::gtest
)

gtest_discover_tests(TestAsyncRestClient)
