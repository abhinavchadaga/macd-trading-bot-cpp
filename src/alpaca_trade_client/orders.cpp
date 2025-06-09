#include "alpaca_trade_client/orders.hpp"

nlohmann::json
market_order::to_json() const
{
  nlohmann::json json {};

  json["symbol"]         = symbol;
  json["qty"]            = qty;
  json["side"]           = side;
  json["type"]           = "market";
  json["time_in_force"]  = time_in_force;
  json["extended_hours"] = extended_hours;

  return json;
}

nlohmann::json
limit_order::to_json() const
{
  nlohmann::json json {};

  json["symbol"]         = symbol;
  json["qty"]            = qty;
  json["side"]           = side;
  json["type"]           = "limit";
  json["limit_price"]    = limit_price;
  json["time_in_force"]  = time_in_force;
  json["extended_hours"] = extended_hours;

  return json;
}
