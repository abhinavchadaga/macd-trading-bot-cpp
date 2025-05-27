add_executable(unit_tests
        test_web_socket_session.cpp
        test_alpaca_ws_market_feed.cpp
)

target_link_libraries(unit_tests
        PRIVATE
        web_socket_session
        alpaca_market_feed
        GTest::gtest_main
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        easyloggingpp
)

target_include_directories(unit_tests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${easyloggingpp_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(unit_tests)
