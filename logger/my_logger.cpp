#include "my_logger.hpp"

#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <format>
#include <string_view>

namespace
{
std::shared_ptr<spdlog::async_logger> global_logger { nullptr };
}

void
init_logger(const std::string_view name)
{
  static bool is_thread_pool_initialized { false };
  if (!is_thread_pool_initialized)
    {
      spdlog::init_thread_pool(10000, 1);
      is_thread_pool_initialized = true;
    }

  if (!global_logger)
    {
      global_logger = make_logger(name);
      spdlog::set_default_logger(global_logger);
      spdlog::set_level(spdlog::level::trace);
    }
}

std::shared_ptr<spdlog::async_logger>
make_logger(std::string_view name)
{
  static bool is_thread_pool_initialized { false };
  if (!is_thread_pool_initialized)
    {
      spdlog::init_thread_pool(10000, 1);
      is_thread_pool_initialized = true;
    }

  std::vector<spdlog::sink_ptr> sinks;
  sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
  sinks.push_back(
    std::make_shared<spdlog::sinks::daily_file_sink_st>(
      std::format("logs/{}/{}.logfile", name, name),
      23,
      59));

  auto logger { std::make_shared<spdlog::async_logger>(
    std::string { name },
    sinks.begin(),
    sinks.end(),
    spdlog::thread_pool(),
    spdlog::async_overflow_policy::block) };
  logger->set_pattern("[%Y-%m-%d %T.%e] [%n] [%^%l%$] [%!] %v");
  return logger;
}
