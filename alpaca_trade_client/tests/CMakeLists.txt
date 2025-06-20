find_package(GTest REQUIRED)
include(GoogleTest)

FILE(GLOB TEST_SRC "test_*.cpp")

#add_executable(alpaca_trade_client_tests
#        ${TEST_SRC}
#)
#
#target_link_libraries(alpaca_trade_client_tests
#        alpaca_trade_client
#        GTest::gtest
#        GTest::gtest_main
#)
#
#gtest_discover_tests(alpaca_trade_client_tests)
