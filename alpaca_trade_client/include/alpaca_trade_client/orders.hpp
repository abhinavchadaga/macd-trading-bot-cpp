//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once

#include "position.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

enum class order_class
{
    SIMPLE,
    BRACKET,
    OCO,
    OTO,
    MLEG
};

enum class order_type
{
    MARKET,
    LIMIT,
    STOP,
    STOP_LIMIT,
    TRAILING_STOP
};

enum class time_in_force
{
    DAY,
    GTC,
    OPG,
    CLS,
    IOC,
    FOK
};

enum class order_status
{
    NEW,
    PARTIALLY_FILLED,
    FILLED,
    DONE_FOR_DAY,
    CANCELED,
    EXPIRED,
    REPLACED,
    PENDING_CANCEL,
    PENDING_REPLACE,
    ACCEPTED,
    PENDING_NEW,
    ACCEPTED_FOR_BIDDING,
    STOPPED,
    REJECTED,
    SUSPENDED,
    CALCULATED
};

enum class position_intent
{
    BUY_TO_OPEN,
    BUY_TO_CLOSE,
    SELL_TO_OPEN,
    SELL_TO_CLOSE
};

enum class order_side
{
    BUY,
    SELL
};

struct order
{
    std::string                       id{};
    std::string                       client_order_id{};
    std::string                       created_at{};
    std::optional<std::string>        updated_at{};
    std::optional<std::string>        submitted_at{};
    std::optional<std::string>        filled_at{};
    std::optional<std::string>        expired_at{};
    std::optional<std::string>        canceled_at{};
    std::optional<std::string>        failed_at{};
    std::optional<std::string>        replaced_at{};
    std::optional<std::string>        replaced_by{};
    std::optional<std::string>        replaces{};
    std::string                       asset_id{};
    std::string                       symbol{};
    asset_class                       asset_class_type{};
    std::optional<std::string>        notional{};
    std::optional<std::string>        qty{};
    std::string                       filled_qty{};
    std::optional<std::string>        filled_avg_price{};
    order_class                       order_class{};
    order_type                        type{};
    order_side                        side{};
    time_in_force                     time_in_force{};
    std::optional<std::string>        limit_price{};
    std::optional<std::string>        stop_price{};
    order_status                      status{};
    bool                              extended_hours{};
    std::optional<std::vector<order>> legs{};
    std::optional<std::string>        trail_percent{};
    std::optional<std::string>        trail_price{};
    std::optional<std::string>        hwm{};
    std::optional<position_intent>    position_intent_type{};
};

struct position_closed
{
    std::string symbol{};
    std::string status{};
    order       order{};
};

struct order_deleted
{
    std::string id{};
    int         status{};
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
    if (const std::string side_str = j; side_str == "buy")
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

inline void to_json(nlohmann::json& j, const order_class& oc)
{
    switch (oc)
    {
        case order_class::SIMPLE:
            j = "simple";
            break;
        case order_class::BRACKET:
            j = "bracket";
            break;
        case order_class::OCO:
            j = "oco";
            break;
        case order_class::OTO:
            j = "oto";
            break;
        case order_class::MLEG:
            j = "mleg";
            break;
    }
}

inline void from_json(const nlohmann::json& j, order_class& oc)
{
    if (const std::string class_str = j; class_str == "simple" || class_str.empty())
        oc = order_class::SIMPLE;
    else if (class_str == "bracket")
        oc = order_class::BRACKET;
    else if (class_str == "oco")
        oc = order_class::OCO;
    else if (class_str == "oto")
        oc = order_class::OTO;
    else if (class_str == "mleg")
        oc = order_class::MLEG;
    else
        throw std::invalid_argument("Invalid order_class: " + class_str);
}

inline void to_json(nlohmann::json& j, const order_type& ot)
{
    switch (ot)
    {
        case order_type::MARKET:
            j = "market";
            break;
        case order_type::LIMIT:
            j = "limit";
            break;
        case order_type::STOP:
            j = "stop";
            break;
        case order_type::STOP_LIMIT:
            j = "stop_limit";
            break;
        case order_type::TRAILING_STOP:
            j = "trailing_stop";
            break;
    }
}

inline void from_json(const nlohmann::json& j, order_type& ot)
{
    if (const std::string type_str = j; type_str == "market")
        ot = order_type::MARKET;
    else if (type_str == "limit")
        ot = order_type::LIMIT;
    else if (type_str == "stop")
        ot = order_type::STOP;
    else if (type_str == "stop_limit")
        ot = order_type::STOP_LIMIT;
    else if (type_str == "trailing_stop")
        ot = order_type::TRAILING_STOP;
    else
        throw std::invalid_argument("Invalid order_type: " + type_str);
}

inline void to_json(nlohmann::json& j, const time_in_force& tif)
{
    switch (tif)
    {
        case time_in_force::DAY:
            j = "day";
            break;
        case time_in_force::GTC:
            j = "gtc";
            break;
        case time_in_force::OPG:
            j = "opg";
            break;
        case time_in_force::CLS:
            j = "cls";
            break;
        case time_in_force::IOC:
            j = "ioc";
            break;
        case time_in_force::FOK:
            j = "fok";
            break;
    }
}

inline void from_json(const nlohmann::json& j, time_in_force& tif)
{
    if (const std::string tif_str = j; tif_str == "day")
        tif = time_in_force::DAY;
    else if (tif_str == "gtc")
        tif = time_in_force::GTC;
    else if (tif_str == "opg")
        tif = time_in_force::OPG;
    else if (tif_str == "cls")
        tif = time_in_force::CLS;
    else if (tif_str == "ioc")
        tif = time_in_force::IOC;
    else if (tif_str == "fok")
        tif = time_in_force::FOK;
    else
        throw std::invalid_argument("Invalid time_in_force: " + tif_str);
}

inline void to_json(nlohmann::json& j, const order_status& os)
{
    switch (os)
    {
        case order_status::NEW:
            j = "new";
            break;
        case order_status::PARTIALLY_FILLED:
            j = "partially_filled";
            break;
        case order_status::FILLED:
            j = "filled";
            break;
        case order_status::DONE_FOR_DAY:
            j = "done_for_day";
            break;
        case order_status::CANCELED:
            j = "canceled";
            break;
        case order_status::EXPIRED:
            j = "expired";
            break;
        case order_status::REPLACED:
            j = "replaced";
            break;
        case order_status::PENDING_CANCEL:
            j = "pending_cancel";
            break;
        case order_status::PENDING_REPLACE:
            j = "pending_replace";
            break;
        case order_status::ACCEPTED:
            j = "accepted";
            break;
        case order_status::PENDING_NEW:
            j = "pending_new";
            break;
        case order_status::ACCEPTED_FOR_BIDDING:
            j = "accepted_for_bidding";
            break;
        case order_status::STOPPED:
            j = "stopped";
            break;
        case order_status::REJECTED:
            j = "rejected";
            break;
        case order_status::SUSPENDED:
            j = "suspended";
            break;
        case order_status::CALCULATED:
            j = "calculated";
            break;
    }
}

inline void from_json(const nlohmann::json& j, order_status& os)
{
    if (const std::string status_str = j; status_str == "new")
        os = order_status::NEW;
    else if (status_str == "partially_filled")
        os = order_status::PARTIALLY_FILLED;
    else if (status_str == "filled")
        os = order_status::FILLED;
    else if (status_str == "done_for_day")
        os = order_status::DONE_FOR_DAY;
    else if (status_str == "canceled")
        os = order_status::CANCELED;
    else if (status_str == "expired")
        os = order_status::EXPIRED;
    else if (status_str == "replaced")
        os = order_status::REPLACED;
    else if (status_str == "pending_cancel")
        os = order_status::PENDING_CANCEL;
    else if (status_str == "pending_replace")
        os = order_status::PENDING_REPLACE;
    else if (status_str == "accepted")
        os = order_status::ACCEPTED;
    else if (status_str == "pending_new")
        os = order_status::PENDING_NEW;
    else if (status_str == "accepted_for_bidding")
        os = order_status::ACCEPTED_FOR_BIDDING;
    else if (status_str == "stopped")
        os = order_status::STOPPED;
    else if (status_str == "rejected")
        os = order_status::REJECTED;
    else if (status_str == "suspended")
        os = order_status::SUSPENDED;
    else if (status_str == "calculated")
        os = order_status::CALCULATED;
    else
        throw std::invalid_argument("Invalid order_status: " + status_str);
}

inline void to_json(nlohmann::json& j, const position_intent& pi)
{
    switch (pi)
    {
        case position_intent::BUY_TO_OPEN:
            j = "buy_to_open";
            break;
        case position_intent::BUY_TO_CLOSE:
            j = "buy_to_close";
            break;
        case position_intent::SELL_TO_OPEN:
            j = "sell_to_open";
            break;
        case position_intent::SELL_TO_CLOSE:
            j = "sell_to_close";
            break;
    }
}

inline void from_json(const nlohmann::json& j, position_intent& pi)
{
    if (const std::string intent_str = j; intent_str == "buy_to_open")
        pi = position_intent::BUY_TO_OPEN;
    else if (intent_str == "buy_to_close")
        pi = position_intent::BUY_TO_CLOSE;
    else if (intent_str == "sell_to_open")
        pi = position_intent::SELL_TO_OPEN;
    else if (intent_str == "sell_to_close")
        pi = position_intent::SELL_TO_CLOSE;
    else
        throw std::invalid_argument("Invalid position_intent: " + intent_str);
}

inline void from_json(const nlohmann::json& j, order& o)
{
    j.at("id").get_to(o.id);
    j.at("client_order_id").get_to(o.client_order_id);
    j.at("created_at").get_to(o.created_at);
    j.at("asset_id").get_to(o.asset_id);
    j.at("symbol").get_to(o.symbol);
    j.at("asset_class").get_to(o.asset_class_type);
    j.at("filled_qty").get_to(o.filled_qty);
    j.at("order_class").get_to(o.order_class);
    j.at("order_type").get_to(o.type);
    j.at("side").get_to(o.side);
    j.at("time_in_force").get_to(o.time_in_force);
    j.at("status").get_to(o.status);
    j.at("extended_hours").get_to(o.extended_hours);

    if (j.contains("updated_at") && !j.at("updated_at").is_null())
        o.updated_at = j.at("updated_at").get<std::string>();
    if (j.contains("submitted_at") && !j.at("submitted_at").is_null())
        o.submitted_at = j.at("submitted_at").get<std::string>();
    if (j.contains("filled_at") && !j.at("filled_at").is_null())
        o.filled_at = j.at("filled_at").get<std::string>();
    if (j.contains("expired_at") && !j.at("expired_at").is_null())
        o.expired_at = j.at("expired_at").get<std::string>();
    if (j.contains("canceled_at") && !j.at("canceled_at").is_null())
        o.canceled_at = j.at("canceled_at").get<std::string>();
    if (j.contains("failed_at") && !j.at("failed_at").is_null())
        o.failed_at = j.at("failed_at").get<std::string>();
    if (j.contains("replaced_at") && !j.at("replaced_at").is_null())
        o.replaced_at = j.at("replaced_at").get<std::string>();
    if (j.contains("replaced_by") && !j.at("replaced_by").is_null())
        o.replaced_by = j.at("replaced_by").get<std::string>();
    if (j.contains("replaces") && !j.at("replaces").is_null())
        o.replaces = j.at("replaces").get<std::string>();
    if (j.contains("notional") && !j.at("notional").is_null())
        o.notional = j.at("notional").get<std::string>();
    if (j.contains("qty") && !j.at("qty").is_null())
        o.qty = j.at("qty").get<std::string>();
    if (j.contains("filled_avg_price") && !j.at("filled_avg_price").is_null())
        o.filled_avg_price = j.at("filled_avg_price").get<std::string>();
    if (j.contains("limit_price") && !j.at("limit_price").is_null())
        o.limit_price = j.at("limit_price").get<std::string>();
    if (j.contains("stop_price") && !j.at("stop_price").is_null())
        o.stop_price = j.at("stop_price").get<std::string>();
    if (j.contains("trail_percent") && !j.at("trail_percent").is_null())
        o.trail_percent = j.at("trail_percent").get<std::string>();
    if (j.contains("trail_price") && !j.at("trail_price").is_null())
        o.trail_price = j.at("trail_price").get<std::string>();
    if (j.contains("hwm") && !j.at("hwm").is_null())
        o.hwm = j.at("hwm").get<std::string>();
    if (j.contains("position_intent") && !j.at("position_intent").is_null())
        o.position_intent_type = j.at("position_intent").get<position_intent>();
    if (j.contains("legs") && !j.at("legs").is_null())
        o.legs = j.at("legs").get<std::vector<order>>();
}

inline void to_json(nlohmann::json& j, const order& o)
{
    j = nlohmann::json{
        {"id", o.id},
        {"client_order_id", o.client_order_id},
        {"created_at", o.created_at},
        {"asset_id", o.asset_id},
        {"symbol", o.symbol},
        {"asset_class", o.asset_class_type},
        {"filled_qty", o.filled_qty},
        {"order_class", o.order_class},
        {"order_type", o.type},
        {"side", o.side},
        {"time_in_force", o.time_in_force},
        {"status", o.status},
        {"extended_hours", o.extended_hours}};

    if (o.updated_at.has_value())
        j["updated_at"] = o.updated_at.value();
    if (o.submitted_at.has_value())
        j["submitted_at"] = o.submitted_at.value();
    if (o.filled_at.has_value())
        j["filled_at"] = o.filled_at.value();
    if (o.expired_at.has_value())
        j["expired_at"] = o.expired_at.value();
    if (o.canceled_at.has_value())
        j["canceled_at"] = o.canceled_at.value();
    if (o.failed_at.has_value())
        j["failed_at"] = o.failed_at.value();
    if (o.replaced_at.has_value())
        j["replaced_at"] = o.replaced_at.value();
    if (o.replaced_by.has_value())
        j["replaced_by"] = o.replaced_by.value();
    if (o.replaces.has_value())
        j["replaces"] = o.replaces.value();
    if (o.notional.has_value())
        j["notional"] = o.notional.value();
    if (o.qty.has_value())
        j["qty"] = o.qty.value();
    if (o.filled_avg_price.has_value())
        j["filled_avg_price"] = o.filled_avg_price.value();
    if (o.limit_price.has_value())
        j["limit_price"] = o.limit_price.value();
    if (o.stop_price.has_value())
        j["stop_price"] = o.stop_price.value();
    if (o.trail_percent.has_value())
        j["trail_percent"] = o.trail_percent.value();
    if (o.trail_price.has_value())
        j["trail_price"] = o.trail_price.value();
    if (o.hwm.has_value())
        j["hwm"] = o.hwm.value();
    if (o.position_intent_type.has_value())
        j["position_intent"] = o.position_intent_type.value();
    if (o.legs.has_value())
        j["legs"] = o.legs.value();
}

// JSON serialization for position_closed struct
inline void from_json(const nlohmann::json& j, position_closed& pc)
{
    j.at("symbol").get_to(pc.symbol);
    j.at("status").get_to(pc.status);
    j.at("body").get_to(pc.order);
}

inline void to_json(nlohmann::json& j, const position_closed& pc)
{
    j = nlohmann::json{{"symbol", pc.symbol}, {"status", pc.status}, {"body", pc.order}};
}

// JSON serialization for order_deleted struct
inline void from_json(const nlohmann::json& j, order_deleted& od)
{
    j.at("id").get_to(od.id);
    j.at("status").get_to(od.status);
}

inline void to_json(nlohmann::json& j, const order_deleted& od)
{
    j = nlohmann::json{{"id", od.id}, {"status", od.status}};
}
