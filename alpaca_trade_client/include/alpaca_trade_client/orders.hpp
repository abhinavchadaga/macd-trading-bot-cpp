//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once

#include <nlohmann/json.hpp>
#include <string>

enum class order_side
{
    BUY,
    SELL
};

struct notional_order
{
    std::string symbol{};
    std::string notional{};
    order_side  side{};
    bool        extended_hours{false};
    std::string client_order_id{};
};

inline void to_json(nlohmann::json& j, const order_side& side)
{
    switch (side)
    {
        case order_side::BUY:
            j = "buy";
            break;
        case order_side::SELL:
            j = "sell";
            break;
    }
}

inline void from_json(const nlohmann::json& j, order_side& side)
{
    const std::string side_str = j.get<std::string>();
    if (side_str == "buy")
    {
        side = order_side::BUY;
    }
    else if (side_str == "sell")
    {
        side = order_side::SELL;
    }
    else
    {
        throw std::invalid_argument("Invalid order_side: " + side_str);
    }
}

inline void to_json(nlohmann::json& j, const notional_order& order)
{
    j = nlohmann::json{
        {"symbol", order.symbol},
        {"notional", order.notional},
        {"side", order.side},
        {"type", "market"},
        {"time_in_force", "day"},
        {"extended_hours", order.extended_hours}};

    if (!order.client_order_id.empty())
    {
        j["client_order_id"] = order.client_order_id;
    }
}

inline void from_json(const nlohmann::json& j, notional_order& order)
{
    j.at("symbol").get_to(order.symbol);
    j.at("notional").get_to(order.notional);
    j.at("side").get_to(order.side);
    j.at("extended_hours").get_to(order.extended_hours);

    if (j.contains("client_order_id"))
    {
        j.at("client_order_id").get_to(order.client_order_id);
    }
}
