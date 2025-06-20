find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(alpaca_trade_client_tests
        TestAlpacaTradeClientIntegration.cpp
)

target_link_libraries(alpaca_trade_client_tests
        alpaca_trade_client
        GTest::gtest_main
)

gtest_discover_tests(alpaca_trade_client_tests)
