find_package(spdlog REQUIRED)
add_library(my_logger STATIC my_logger.cpp)
target_link_libraries(my_logger PUBLIC spdlog::spdlog)
