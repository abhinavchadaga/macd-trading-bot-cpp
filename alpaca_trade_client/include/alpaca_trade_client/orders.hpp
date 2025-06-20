//
// Created by Abhinav  Chadaga on 6/19/25.
//

#pragma once

#include "position.hpp"
#include <boost/json.hpp>
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
    int         status{};
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

void       tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order_side& side);
order_side tag_invoke(boost::json::value_to_tag<order_side>, const boost::json::value& jv);

void        tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order_class& oc);
order_class tag_invoke(boost::json::value_to_tag<order_class>, const boost::json::value& jv);

void       tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order_type& ot);
order_type tag_invoke(boost::json::value_to_tag<order_type>, const boost::json::value& jv);

void          tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const time_in_force& tif);
time_in_force tag_invoke(boost::json::value_to_tag<time_in_force>, const boost::json::value& jv);

void         tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order_status& os);
order_status tag_invoke(boost::json::value_to_tag<order_status>, const boost::json::value& jv);

void            tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const position_intent& pi);
position_intent tag_invoke(boost::json::value_to_tag<position_intent>, const boost::json::value& jv);

void  tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order& o);
order tag_invoke(boost::json::value_to_tag<order>, const boost::json::value& jv);

void           tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const notional_order& order);
notional_order tag_invoke(boost::json::value_to_tag<notional_order>, const boost::json::value& jv);

void            tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const position_closed& pc);
position_closed tag_invoke(boost::json::value_to_tag<position_closed>, const boost::json::value& jv);

void          tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const order_deleted& od);
order_deleted tag_invoke(boost::json::value_to_tag<order_deleted>, const boost::json::value& jv);
