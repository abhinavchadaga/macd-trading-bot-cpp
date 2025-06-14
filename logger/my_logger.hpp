//
// Created by Abhinav  Chadaga on 6/14/25.
//

#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_FUNCTION     __PRETTY_FUNCTION__

#include "spdlog/async.h"
#include "spdlog/spdlog.h"

#include <memory>
#include <string_view>

void init_logger(std::string_view name);

#define LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

std::shared_ptr<spdlog::async_logger> make_logger(std::string_view name);
