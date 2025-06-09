#include "LoggingUtils.hpp"

void
configure_logging()
{
  el::Configurations conf;

  conf.set(
    el::Level::Global,
    el::ConfigurationType::Format,
    "%datetime %level [MACDTradingBot]%msg");
  conf.set(el::Level::Global, el::ConfigurationType::ToFile, "false");
  conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
  conf.set(el::Level::Global, el::ConfigurationType::SubsecondPrecision, "0");
  conf.set(
    el::Level::Global,
    el::ConfigurationType::PerformanceTracking,
    "false");
  conf.set(el::Level::Global, el::ConfigurationType::MaxLogFileSize, "0");
  conf.set(el::Level::Global, el::ConfigurationType::LogFlushThreshold, "0");

  el::Loggers::reconfigureLogger("MACDTradingBot", conf);
  el::Loggers::getLogger("MACDTradingBot");
}
