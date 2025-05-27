# WebSocket session library
add_library(web_socket_session
    web_socket_session.cpp
)

target_include_directories(web_socket_session
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(web_socket_session
    PUBLIC
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_compile_features(web_socket_session PUBLIC cxx_std_20)

# Alpaca market feed library
add_library(alpaca_market_feed AlpacaWSMarketFeed.cpp)

target_include_directories(alpaca_market_feed PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${easyloggingpp_SOURCE_DIR}/src
)

target_link_libraries(alpaca_market_feed PUBLIC
    Boost::asio
    Boost::beast
    Boost::system
    nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    easyloggingpp
)
