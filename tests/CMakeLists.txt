include(GoogleTest)
set(TEST_FILES
    TestWebSocketSession.cpp
    TestAlpacaWSMarketFeed.cpp
    TestBarAggregatorIntegration.cpp
    TestBar.cpp
    TestUtils.cpp
    TestIndicators.cpp
    TestIndicatorEngine.cpp)

foreach(TEST_FILE ${TEST_FILES})
  get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
  add_executable(${TEST_NAME} ${TEST_FILE})
  target_link_libraries(${TEST_NAME} PUBLIC macd-trading-bot GTest::gtest_main)
  target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include
                                                 ${CMAKE_CURRENT_SOURCE_DIR})
  gtest_discover_tests(${TEST_NAME})
endforeach()

pkg_check_modules(TALIB REQUIRED IMPORTED_TARGET ta-lib)
target_link_libraries(TestIndicators PUBLIC PkgConfig::TALIB)
