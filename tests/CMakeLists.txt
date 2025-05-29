add_executable(unit_tests
        TestWebSocketSession.cpp
        TestAlpacaWSMarketFeed.cpp
        TestBarAggregatorIntegration.cpp
)
target_link_libraries(unit_tests
        PUBLIC
        easylogging
        LoggingUtils
        WebSocketSession
        AlpacaMarketFeed
        BarAggregator
        GTest::gtest_main
)

target_include_directories(unit_tests
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
