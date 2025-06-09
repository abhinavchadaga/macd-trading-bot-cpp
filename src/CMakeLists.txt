# Collect all source files
file(GLOB_RECURSE MACD_TRADING_BOT_SRCS 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/indicators/*.cpp"
)

# Create the main static library
add_library(macd-trading-bot STATIC ${MACD_TRADING_BOT_SRCS})

# Link all required dependencies
target_link_libraries(macd-trading-bot 
    PUBLIC
    easylogging
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
    nlohmann_json::nlohmann_json
)

# Set include directories
target_include_directories(macd-trading-bot 
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third-party
)
