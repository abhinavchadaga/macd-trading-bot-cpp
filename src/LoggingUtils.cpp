#include "LoggingUtils.hpp"

void
configure_logging()
{
  el::Configurations default_conf;
  default_conf.setToDefault();

  default_conf.set(
    el::Level::Global,
    el::ConfigurationType::Format,
    "%datetime %level %msg");
  default_conf.set(el::Level::Global, el::ConfigurationType::ToFile, "false");
  default_conf.set(
    el::Level::Global,
    el::ConfigurationType::ToStandardOutput,
    "true");

  el::Loggers::reconfigureAllLoggers(default_conf);
  el::Loggers::getLogger("MACDTradingBot");
}
