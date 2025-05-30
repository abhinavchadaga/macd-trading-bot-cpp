add_executable(unit_tests
        TestWebSocketSession.cpp
        TestAlpacaWSMarketFeed.cpp
        TestBarAggregatorIntegration.cpp
        TestBar.cpp
        TestUtils.cpp
        TestIndicators.cpp
)
target_link_libraries(unit_tests
        PUBLIC
        easylogging
        LoggingUtils
        WebSocketSession
        AlpacaMarketFeed
        Utils
        Indicators
        GTest::gtest_main
)

pkg_check_modules(TALIB REQUIRED IMPORTED_TARGET ta-lib)
target_link_libraries(unit_tests PUBLIC PkgConfig::TALIB)

target_include_directories(unit_tests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
