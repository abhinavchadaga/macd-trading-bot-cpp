find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(TestAsyncRestClient
        TestAsyncRestClientBasic.cpp
        TestAsyncRestClientUtils.cpp
)

target_link_libraries(TestAsyncRestClient
        async_rest_client
        GTest::gtest_main
        GTest::gtest
)

gtest_discover_tests(TestAsyncRestClient)

add_executable(TestAsyncRestClientGet
        TestAsyncRestClientGet.cpp
)

target_link_libraries(TestAsyncRestClientGet
        async_rest_client
        nlohmann_json::nlohmann_json
        GTest::gtest_main
        GTest::gtest
)

gtest_discover_tests(TestAsyncRestClientGet)
