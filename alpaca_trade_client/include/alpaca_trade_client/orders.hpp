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
    order_class                       order_class_type{};
    order_type                        type{};
    order_side                        side{};
    time_in_force                     time_in_force_type{};
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
    order       order_details{};
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

void to_json(nlohmann::json& j, const order_side& side);
void from_json(const nlohmann::json& j, order_side& side);

void to_json(nlohmann::json& j, const order_class& oc);
void from_json(const nlohmann::json& j, order_class& oc);

void to_json(nlohmann::json& j, const order_type& ot);
void from_json(const nlohmann::json& j, order_type& ot);

void to_json(nlohmann::json& j, const time_in_force& tif);
void from_json(const nlohmann::json& j, time_in_force& tif);

void to_json(nlohmann::json& j, const order_status& os);
void from_json(const nlohmann::json& j, order_status& os);

void to_json(nlohmann::json& j, const position_intent& pi);
void from_json(const nlohmann::json& j, position_intent& pi);

void from_json(const nlohmann::json& j, order& o);
void to_json(nlohmann::json& j, const order& o);

void to_json(nlohmann::json& j, const notional_order& order);
void from_json(const nlohmann::json& j, notional_order& order);

void from_json(const nlohmann::json& j, position_closed& pc);
void to_json(nlohmann::json& j, const position_closed& pc);

void from_json(const nlohmann::json& j, order_deleted& od);
void to_json(nlohmann::json& j, const order_deleted& od);
