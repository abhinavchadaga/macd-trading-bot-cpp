#include "my_logger.hpp"

#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <format>
#include <regex>
#include <string_view>

namespace
{
std::shared_ptr<spdlog::async_logger> global_logger { nullptr };

// TODO (achadaga): verify this
class function_formatter_flag final : public spdlog::custom_flag_formatter
{
public:

  void
  format(
    const spdlog::details::log_msg &msg,
    const std::tm &,
    spdlog::memory_buf_t &dest) override
  {
    if (!msg.source.funcname)
      {
        return;
      }

    std::string full_func(msg.source.funcname);

    // Extract class::function from various function signature formats
    // Handles patterns like:
    // - "ReturnType NameSpace::Class::Function(Args)"
    // - "ReturnType Class::Function(Args)"
    // - "ReturnType Function(Args)"

    // Find the last occurrence of space before the function name
    size_t last_space = full_func.find_last_of(' ');
    if (last_space != std::string::npos)
      {
        // Extract everything after the return type
        std::string func_part = full_func.substr(last_space + 1);

        // Find the opening parenthesis
        size_t paren_pos = func_part.find('(');
        if (paren_pos != std::string::npos)
          {
            // Extract just the namespace::class::function part
            std::string name_part = func_part.substr(0, paren_pos);

            // If there's a namespace, extract just class::function
            size_t last_ns_sep = name_part.rfind("::");
            if (last_ns_sep != std::string::npos)
              {
                // Find the second-to-last :: to get class::function
                size_t second_last = name_part.rfind("::", last_ns_sep - 1);
                if (second_last != std::string::npos)
                  {
                    dest.append(name_part.substr(second_last + 2));
                  }
                else
                  {
                    dest.append(name_part);
                  }
              }
            else
              {
                dest.append(name_part);
              }
            return;
          }
      }

    // Fallback: use the original function name
    dest.append(full_func);
  }

  [[nodiscard]]
  std::unique_ptr<custom_flag_formatter>
  clone() const override
  {
    return std::make_unique<function_formatter_flag>();
  }
};
} // namespace

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

  // Stdout sink - log debug and up
  auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
  stdout_sink->set_level(spdlog::level::debug);
  sinks.push_back(stdout_sink);

  // File sink - log trace and up (everything)
  auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_st>(
    std::format("logs/{}/{}.logfile", name, name),
    23,
    59);
  file_sink->set_level(spdlog::level::trace);
  sinks.push_back(file_sink);

  auto logger { std::make_shared<spdlog::async_logger>(
    std::string { name },
    sinks.begin(),
    sinks.end(),
    spdlog::thread_pool(),
    spdlog::async_overflow_policy::block) };

  // Create custom formatter with our function formatter
  auto formatter = std::make_unique<spdlog::pattern_formatter>();
  formatter->add_flag<function_formatter_flag>('F');
  formatter->set_pattern("[%Y-%m-%d %T.%e] [%n] [%^%l%$] [%F] %v");
  logger->set_formatter(std::move(formatter));

  return logger;
}
