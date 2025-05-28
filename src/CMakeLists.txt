add_library(LoggingUtils LoggingUtils.cpp)
target_include_directories(LoggingUtils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(LoggingUtils easylogging)

add_library(WebSocketSession WebSocketSession.cpp)
target_include_directories(WebSocketSession
        PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(WebSocketSession
        PUBLIC
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        LoggingUtils
)

add_library(AlpacaMarketFeed AlpacaWSMarketFeed.cpp)
target_include_directories(AlpacaMarketFeed PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(AlpacaMarketFeed PUBLIC
        WebSocketSession
        nlohmann_json::nlohmann_json
        LoggingUtils
)

add_library(BarAggregator BarAggregator.cpp)
target_include_directories(BarAggregator PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(BarAggregator PUBLIC
        Boost::system
)
