#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace OrderSide
{
inline constexpr const char *BUY  = "buy";
inline constexpr const char *SELL = "sell";
} // namespace OrderSide

namespace OrderType
{
inline constexpr const char *MARKET = "market";
inline constexpr const char *LIMIT  = "limit";
} // namespace OrderType

namespace TimeInForce
{
inline constexpr const char *DAY = "day";
inline constexpr const char *GTC = "gtc";
inline constexpr const char *IOC = "ioc";
inline constexpr const char *FOK = "fok";
} // namespace TimeInForce

class market_order
{
public:

  std::string symbol;
  std::string qty;
  std::string side;
  std::string time_in_force { TimeInForce::DAY };
  bool        extended_hours { false };

  nlohmann::json to_json() const;
};

class limit_order
{
public:

  std::string symbol;
  std::string qty;
  std::string side;
  std::string limit_price;
  std::string time_in_force { TimeInForce::DAY };
  bool        extended_hours { false };

  nlohmann::json to_json() const;
};
