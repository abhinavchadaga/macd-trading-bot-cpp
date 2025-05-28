#pragma once

#include "../third-party/easylogging++.h"

#define CLASS_LOGGER(class_name)                                              \
  el::Logger *_class_logger = el::Loggers::getLogger (#class_name)

#define FUNC_LOG(level, class_name, func_name)                                \
  CLOG (level, "main") << "[" << #class_name << "::" << #func_name << "] "

#define LOG_INFO(class_name, func_name) FUNC_LOG (INFO, class_name, func_name)

#define LOG_ERROR(class_name, func_name)                                      \
  FUNC_LOG (ERROR, class_name, func_name)

#define LOG_DEBUG(class_name, func_name)                                      \
  FUNC_LOG (DEBUG, class_name, func_name)

#define LOG_WARNING(class_name, func_name)                                    \
  FUNC_LOG (WARNING, class_name, func_name)

void configure_logging ();
