enable_testing()

# Test executable
add_executable(unit_tests
        test_web_socket_session.cpp
)

# Link against source libraries and dependencies
target_link_libraries(unit_tests
        PRIVATE
        web_socket_session
        GTest::gtest_main
        Boost::asio
        Boost::beast
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
        project_warnings
)

# Include directories for headers
target_include_directories(unit_tests
        PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# Add test discovery
include(GoogleTest)
gtest_discover_tests(unit_tests)
