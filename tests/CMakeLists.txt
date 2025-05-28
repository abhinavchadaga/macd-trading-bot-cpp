add_executable(unit_tests
        TestWebSocketSession.cpp
        TestAlpacaWSMarketFeed.cpp
)
target_link_libraries(unit_tests
        PRIVATE
        WebSocketSession
        AlpacaMarketFeed
        GTest::gtest_main
)

target_include_directories(unit_tests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
