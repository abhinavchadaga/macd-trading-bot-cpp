add_library(LoggingUtils OBJECT LoggingUtils.cpp)
target_link_libraries(LoggingUtils PUBLIC easylogging)

add_library(WebSocketSession OBJECT WebSocketSession.cpp)
target_link_libraries(WebSocketSession
        PUBLIC
        Boost::system
        OpenSSL::SSL
        OpenSSL::Crypto
        LoggingUtils
)

add_library(Utils OBJECT Utils.cpp)

add_library(AlpacaMarketFeed OBJECT AlpacaWSMarketFeed.cpp)
target_link_libraries(AlpacaMarketFeed PUBLIC WebSocketSession nlohmann_json::nlohmann_json Utils)
