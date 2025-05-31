#include "LoggingUtils.hpp"

void configure_logging() {
  el::Configurations defaultConf;
  defaultConf.setToDefault();

  defaultConf.set(el::Level::Global, el::ConfigurationType::Format,
                  "%datetime %level %msg");
  defaultConf.set(el::Level::Global, el::ConfigurationType::ToFile, "false");
  defaultConf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput,
                  "true");

  el::Loggers::reconfigureAllLoggers(defaultConf);
  el::Loggers::getLogger("MACDTradingBot");
}
