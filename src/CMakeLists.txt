add_executable(main main.cpp)
target_link_libraries(main PRIVATE Boost::beast Boost::asio Boost::system nlohmann_json::nlohmann_json)