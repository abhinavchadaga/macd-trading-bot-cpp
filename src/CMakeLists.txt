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

file(GLOB INDICATORS_SRCS indicators/*.cpp)
add_library(Indicators OBJECT ${INDICATORS_SRCS})
target_include_directories(Indicators PUBLIC ${CMAKE_SOURCE_DIR}/include)
