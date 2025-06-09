# List of test files
set(TEST_FILES
    TestWebSocketSession.cpp
    TestAlpacaWSMarketFeed.cpp
    TestBarAggregatorIntegration.cpp
    TestBar.cpp
    TestUtils.cpp
    TestIndicators.cpp
    TestIndicatorEngine.cpp
)

# Create a separate executable for each test file
foreach(TEST_FILE ${TEST_FILES})
    # Get the test name without extension
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    
    # Create executable for this test
    add_executable(${TEST_NAME} ${TEST_FILE})
    
    # Link against the main static library and GTest
    target_link_libraries(${TEST_NAME}
        PUBLIC
        macd-trading-bot
        GTest::gtest_main
    )
    
    # Set include directories
    target_include_directories(${TEST_NAME}
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    # Register the test with CTest
    gtest_discover_tests(${TEST_NAME})
endforeach()

# Include GoogleTest for test discovery
include(GoogleTest)
