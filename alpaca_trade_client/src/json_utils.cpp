#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/enum_serialization.hpp"
#include "alpaca_trade_client/orders.hpp"
#include "alpaca_trade_client/position.hpp"

//
// account.hpp JSON serialization implementations
//

namespace json = boost::json;

void tag_invoke(json::value_from_tag, json::value& jv, const account_status& status)
{
    enum_value_from_tag(jv, status);
}

account_status tag_invoke(json::value_to_tag<account_status>, const json::value& jv)
{
    return enum_value_to_tag<account_status>(jv);
}

trade_account tag_invoke(json::value_to_tag<trade_account>, const json::value& jv)
{
    const auto&   obj = jv.as_object();
    trade_account account;

    account.id       = json::value_to<std::string>(obj.at("id"));
    account.status   = json::value_to<account_status>(obj.at("status"));
    account.currency = json::value_to<std::string>(obj.at("currency"));

    if (obj.contains("account_number") && !obj.at("account_number").is_null())
        account.account_number = json::value_to<std::string>(obj.at("account_number"));
    if (obj.contains("cash") && !obj.at("cash").is_null())
        account.cash = json::value_to<std::string>(obj.at("cash"));
    if (obj.contains("portfolio_value") && !obj.at("portfolio_value").is_null())
        account.portfolio_value = json::value_to<std::string>(obj.at("portfolio_value"));
    if (obj.contains("non_marginable_buying_power") && !obj.at("non_marginable_buying_power").is_null())
        account.non_marginable_buying_power = json::value_to<std::string>(obj.at("non_marginable_buying_power"));
    if (obj.contains("accrued_fees") && !obj.at("accrued_fees").is_null())
        account.accrued_fees = json::value_to<std::string>(obj.at("accrued_fees"));
    if (obj.contains("pending_transfer_in") && !obj.at("pending_transfer_in").is_null())
        account.pending_transfer_in = json::value_to<std::string>(obj.at("pending_transfer_in"));
    if (obj.contains("pending_transfer_out") && !obj.at("pending_transfer_out").is_null())
        account.pending_transfer_out = json::value_to<std::string>(obj.at("pending_transfer_out"));
    if (obj.contains("created_at") && !obj.at("created_at").is_null())
        account.created_at = json::value_to<std::string>(obj.at("created_at"));
    if (obj.contains("long_market_value") && !obj.at("long_market_value").is_null())
        account.long_market_value = json::value_to<std::string>(obj.at("long_market_value"));
    if (obj.contains("short_market_value") && !obj.at("short_market_value").is_null())
        account.short_market_value = json::value_to<std::string>(obj.at("short_market_value"));
    if (obj.contains("equity") && !obj.at("equity").is_null())
        account.equity = json::value_to<std::string>(obj.at("equity"));
    if (obj.contains("last_equity") && !obj.at("last_equity").is_null())
        account.last_equity = json::value_to<std::string>(obj.at("last_equity"));
    if (obj.contains("multiplier") && !obj.at("multiplier").is_null())
        account.multiplier = json::value_to<std::string>(obj.at("multiplier"));
    if (obj.contains("buying_power") && !obj.at("buying_power").is_null())
        account.buying_power = json::value_to<std::string>(obj.at("buying_power"));
    if (obj.contains("initial_margin") && !obj.at("initial_margin").is_null())
        account.initial_margin = json::value_to<std::string>(obj.at("initial_margin"));
    if (obj.contains("maintenance_margin") && !obj.at("maintenance_margin").is_null())
        account.maintenance_margin = json::value_to<std::string>(obj.at("maintenance_margin"));
    if (obj.contains("sma") && !obj.at("sma").is_null())
        account.sma = json::value_to<std::string>(obj.at("sma"));
    if (obj.contains("balance_asof") && !obj.at("balance_asof").is_null())
        account.balance_asof = json::value_to<std::string>(obj.at("balance_asof"));
    if (obj.contains("last_maintenance_margin") && !obj.at("last_maintenance_margin").is_null())
        account.last_maintenance_margin = json::value_to<std::string>(obj.at("last_maintenance_margin"));
    if (obj.contains("daytrading_buying_power") && !obj.at("daytrading_buying_power").is_null())
        account.daytrading_buying_power = json::value_to<std::string>(obj.at("daytrading_buying_power"));
    if (obj.contains("regt_buying_power") && !obj.at("regt_buying_power").is_null())
        account.regt_buying_power = json::value_to<std::string>(obj.at("regt_buying_power"));
    if (obj.contains("options_buying_power") && !obj.at("options_buying_power").is_null())
        account.options_buying_power = json::value_to<std::string>(obj.at("options_buying_power"));
    if (obj.contains("intraday_adjustments") && !obj.at("intraday_adjustments").is_null())
        account.intraday_adjustments = json::value_to<std::string>(obj.at("intraday_adjustments"));
    if (obj.contains("pending_reg_taf_fees") && !obj.at("pending_reg_taf_fees").is_null())
        account.pending_reg_taf_fees = json::value_to<std::string>(obj.at("pending_reg_taf_fees"));
    if (obj.contains("daytrade_count") && !obj.at("daytrade_count").is_null())
        account.daytrade_count = json::value_to<int>(obj.at("daytrade_count"));
    if (obj.contains("options_approved_level") && !obj.at("options_approved_level").is_null())
        account.options_approved_level = json::value_to<int>(obj.at("options_approved_level"));
    if (obj.contains("options_trading_level") && !obj.at("options_trading_level").is_null())
        account.options_trading_level = json::value_to<int>(obj.at("options_trading_level"));
    if (obj.contains("pattern_day_trader") && !obj.at("pattern_day_trader").is_null())
        account.pattern_day_trader = json::value_to<bool>(obj.at("pattern_day_trader"));
    if (obj.contains("trade_suspended_by_user") && !obj.at("trade_suspended_by_user").is_null())
        account.trade_suspended_by_user = json::value_to<bool>(obj.at("trade_suspended_by_user"));
    if (obj.contains("trading_blocked") && !obj.at("trading_blocked").is_null())
        account.trading_blocked = json::value_to<bool>(obj.at("trading_blocked"));
    if (obj.contains("transfers_blocked") && !obj.at("transfers_blocked").is_null())
        account.transfers_blocked = json::value_to<bool>(obj.at("transfers_blocked"));
    if (obj.contains("account_blocked") && !obj.at("account_blocked").is_null())
        account.account_blocked = json::value_to<bool>(obj.at("account_blocked"));
    if (obj.contains("shorting_enabled") && !obj.at("shorting_enabled").is_null())
        account.shorting_enabled = json::value_to<bool>(obj.at("shorting_enabled"));

    return account;
}

void tag_invoke(json::value_from_tag, json::value& jv, const trade_account& account)
{
    json::object obj;
    obj["id"]       = account.id;
    obj["currency"] = account.currency;
    obj["status"]   = json::value_from(account.status);

    if (account.account_number.has_value())
        obj["account_number"] = account.account_number.value();
    if (account.cash.has_value())
        obj["cash"] = account.cash.value();
    if (account.portfolio_value.has_value())
        obj["portfolio_value"] = account.portfolio_value.value();
    if (account.non_marginable_buying_power.has_value())
        obj["non_marginable_buying_power"] = account.non_marginable_buying_power.value();
    if (account.accrued_fees.has_value())
        obj["accrued_fees"] = account.accrued_fees.value();
    if (account.pending_transfer_in.has_value())
        obj["pending_transfer_in"] = account.pending_transfer_in.value();
    if (account.pending_transfer_out.has_value())
        obj["pending_transfer_out"] = account.pending_transfer_out.value();
    if (account.created_at.has_value())
        obj["created_at"] = account.created_at.value();
    if (account.long_market_value.has_value())
        obj["long_market_value"] = account.long_market_value.value();
    if (account.short_market_value.has_value())
        obj["short_market_value"] = account.short_market_value.value();
    if (account.equity.has_value())
        obj["equity"] = account.equity.value();
    if (account.last_equity.has_value())
        obj["last_equity"] = account.last_equity.value();
    if (account.multiplier.has_value())
        obj["multiplier"] = account.multiplier.value();
    if (account.buying_power.has_value())
        obj["buying_power"] = account.buying_power.value();
    if (account.initial_margin.has_value())
        obj["initial_margin"] = account.initial_margin.value();
    if (account.maintenance_margin.has_value())
        obj["maintenance_margin"] = account.maintenance_margin.value();
    if (account.sma.has_value())
        obj["sma"] = account.sma.value();
    if (account.balance_asof.has_value())
        obj["balance_asof"] = account.balance_asof.value();
    if (account.last_maintenance_margin.has_value())
        obj["last_maintenance_margin"] = account.last_maintenance_margin.value();
    if (account.daytrading_buying_power.has_value())
        obj["daytrading_buying_power"] = account.daytrading_buying_power.value();
    if (account.regt_buying_power.has_value())
        obj["regt_buying_power"] = account.regt_buying_power.value();
    if (account.options_buying_power.has_value())
        obj["options_buying_power"] = account.options_buying_power.value();
    if (account.intraday_adjustments.has_value())
        obj["intraday_adjustments"] = account.intraday_adjustments.value();
    if (account.pending_reg_taf_fees.has_value())
        obj["pending_reg_taf_fees"] = account.pending_reg_taf_fees.value();
    if (account.daytrade_count.has_value())
        obj["daytrade_count"] = account.daytrade_count.value();
    if (account.options_approved_level.has_value())
        obj["options_approved_level"] = account.options_approved_level.value();
    if (account.options_trading_level.has_value())
        obj["options_trading_level"] = account.options_trading_level.value();
    if (account.pattern_day_trader.has_value())
        obj["pattern_day_trader"] = account.pattern_day_trader.value();
    if (account.trade_suspended_by_user.has_value())
        obj["trade_suspended_by_user"] = account.trade_suspended_by_user.value();
    if (account.trading_blocked.has_value())
        obj["trading_blocked"] = account.trading_blocked.value();
    if (account.transfers_blocked.has_value())
        obj["transfers_blocked"] = account.transfers_blocked.value();
    if (account.account_blocked.has_value())
        obj["account_blocked"] = account.account_blocked.value();
    if (account.shorting_enabled.has_value())
        obj["shorting_enabled"] = account.shorting_enabled.value();

    jv = std::move(obj);
}

//
// position.hpp JSON serialization implementations
//

void tag_invoke(json::value_from_tag, json::value& jv, const asset_exchange& exchange)
{
    enum_value_from_tag(jv, exchange);
}

asset_exchange tag_invoke(json::value_to_tag<asset_exchange>, const json::value& jv)
{
    return enum_value_to_tag<asset_exchange>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const asset_class& asset_class_type)
{
    enum_value_from_tag(jv, asset_class_type);
}

asset_class tag_invoke(json::value_to_tag<asset_class>, const json::value& jv)
{
    return enum_value_to_tag<asset_class>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const position_side& side)
{
    enum_value_from_tag(jv, side);
}

position_side tag_invoke(json::value_to_tag<position_side>, const json::value& jv)
{
    return enum_value_to_tag<position_side>(jv);
}

position tag_invoke(json::value_to_tag<position>, const json::value& jv)
{
    const auto& obj = jv.as_object();
    position    pos;

    pos.asset_id                 = json::value_to<std::string>(obj.at("asset_id"));
    pos.symbol                   = json::value_to<std::string>(obj.at("symbol"));
    pos.exchange                 = json::value_to<asset_exchange>(obj.at("exchange"));
    pos.asset_class_type         = json::value_to<asset_class>(obj.at("asset_class"));
    pos.avg_entry_price          = json::value_to<std::string>(obj.at("avg_entry_price"));
    pos.qty                      = json::value_to<std::string>(obj.at("qty"));
    pos.side                     = json::value_to<position_side>(obj.at("side"));
    pos.market_value             = json::value_to<std::string>(obj.at("market_value"));
    pos.cost_basis               = json::value_to<std::string>(obj.at("cost_basis"));
    pos.unrealized_pl            = json::value_to<std::string>(obj.at("unrealized_pl"));
    pos.unrealized_plpc          = json::value_to<std::string>(obj.at("unrealized_plpc"));
    pos.unrealized_intraday_pl   = json::value_to<std::string>(obj.at("unrealized_intraday_pl"));
    pos.unrealized_intraday_plpc = json::value_to<std::string>(obj.at("unrealized_intraday_plpc"));
    pos.current_price            = json::value_to<std::string>(obj.at("current_price"));
    pos.lastday_price            = json::value_to<std::string>(obj.at("lastday_price"));
    pos.change_today             = json::value_to<std::string>(obj.at("change_today"));
    pos.asset_marginable         = json::value_to<bool>(obj.at("asset_marginable"));

    if (obj.contains("qty_available") && !obj.at("qty_available").is_null())
        pos.qty_available = json::value_to<std::string>(obj.at("qty_available"));

    return pos;
}

void tag_invoke(json::value_from_tag, json::value& jv, const position& pos)
{
    json::object obj;
    obj["asset_id"]                 = pos.asset_id;
    obj["symbol"]                   = pos.symbol;
    obj["exchange"]                 = json::value_from(pos.exchange);
    obj["asset_class"]              = json::value_from(pos.asset_class_type);
    obj["avg_entry_price"]          = pos.avg_entry_price;
    obj["qty"]                      = pos.qty;
    obj["side"]                     = json::value_from(pos.side);
    obj["market_value"]             = pos.market_value;
    obj["cost_basis"]               = pos.cost_basis;
    obj["unrealized_pl"]            = pos.unrealized_pl;
    obj["unrealized_plpc"]          = pos.unrealized_plpc;
    obj["unrealized_intraday_pl"]   = pos.unrealized_intraday_pl;
    obj["unrealized_intraday_plpc"] = pos.unrealized_intraday_plpc;
    obj["current_price"]            = pos.current_price;
    obj["lastday_price"]            = pos.lastday_price;
    obj["change_today"]             = pos.change_today;
    obj["asset_marginable"]         = pos.asset_marginable;

    if (pos.qty_available.has_value())
        obj["qty_available"] = pos.qty_available.value();

    jv = std::move(obj);
}

//
// orders.hpp JSON serialization implementations
//

void tag_invoke(json::value_from_tag, json::value& jv, const order_side& side)
{
    enum_value_from_tag(jv, side);
}

order_side tag_invoke(json::value_to_tag<order_side>, const json::value& jv)
{
    return enum_value_to_tag<order_side>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_class& oc)
{
    enum_value_from_tag(jv, oc);
}

order_class tag_invoke(json::value_to_tag<order_class>, const json::value& jv)
{
    return enum_value_to_tag_with_fallback<order_class>(jv, order_class::SIMPLE);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_type& ot)
{
    enum_value_from_tag(jv, ot);
}

order_type tag_invoke(json::value_to_tag<order_type>, const json::value& jv)
{
    return enum_value_to_tag<order_type>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const time_in_force& tif)
{
    enum_value_from_tag(jv, tif);
}

time_in_force tag_invoke(json::value_to_tag<time_in_force>, const json::value& jv)
{
    return enum_value_to_tag<time_in_force>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_status& os)
{
    enum_value_from_tag(jv, os);
}

order_status tag_invoke(json::value_to_tag<order_status>, const json::value& jv)
{
    return enum_value_to_tag<order_status>(jv);
}

void tag_invoke(json::value_from_tag, json::value& jv, const position_intent& pi)
{
    enum_value_from_tag(jv, pi);
}

position_intent tag_invoke(json::value_to_tag<position_intent>, const json::value& jv)
{
    return enum_value_to_tag<position_intent>(jv);
}

order tag_invoke(json::value_to_tag<order>, const json::value& jv)
{
    const auto& obj = jv.as_object();
    order       o;

    o.id                 = json::value_to<std::string>(obj.at("id"));
    o.client_order_id    = json::value_to<std::string>(obj.at("client_order_id"));
    o.created_at         = json::value_to<std::string>(obj.at("created_at"));
    o.asset_id           = json::value_to<std::string>(obj.at("asset_id"));
    o.symbol             = json::value_to<std::string>(obj.at("symbol"));
    o.asset_class_type   = json::value_to<asset_class>(obj.at("asset_class"));
    o.filled_qty         = json::value_to<std::string>(obj.at("filled_qty"));
    o.order_class_type   = json::value_to<order_class>(obj.at("order_class"));
    o.type               = json::value_to<order_type>(obj.at("order_type"));
    o.side               = json::value_to<order_side>(obj.at("side"));
    o.time_in_force_type = json::value_to<time_in_force>(obj.at("time_in_force"));
    o.status             = json::value_to<order_status>(obj.at("status"));
    o.extended_hours     = json::value_to<bool>(obj.at("extended_hours"));

    if (obj.contains("updated_at") && !obj.at("updated_at").is_null())
        o.updated_at = json::value_to<std::string>(obj.at("updated_at"));
    if (obj.contains("submitted_at") && !obj.at("submitted_at").is_null())
        o.submitted_at = json::value_to<std::string>(obj.at("submitted_at"));
    if (obj.contains("filled_at") && !obj.at("filled_at").is_null())
        o.filled_at = json::value_to<std::string>(obj.at("filled_at"));
    if (obj.contains("expired_at") && !obj.at("expired_at").is_null())
        o.expired_at = json::value_to<std::string>(obj.at("expired_at"));
    if (obj.contains("canceled_at") && !obj.at("canceled_at").is_null())
        o.canceled_at = json::value_to<std::string>(obj.at("canceled_at"));
    if (obj.contains("failed_at") && !obj.at("failed_at").is_null())
        o.failed_at = json::value_to<std::string>(obj.at("failed_at"));
    if (obj.contains("replaced_at") && !obj.at("replaced_at").is_null())
        o.replaced_at = json::value_to<std::string>(obj.at("replaced_at"));
    if (obj.contains("replaced_by") && !obj.at("replaced_by").is_null())
        o.replaced_by = json::value_to<std::string>(obj.at("replaced_by"));
    if (obj.contains("replaces") && !obj.at("replaces").is_null())
        o.replaces = json::value_to<std::string>(obj.at("replaces"));
    if (obj.contains("notional") && !obj.at("notional").is_null())
        o.notional = json::value_to<std::string>(obj.at("notional"));
    if (obj.contains("qty") && !obj.at("qty").is_null())
        o.qty = json::value_to<std::string>(obj.at("qty"));
    if (obj.contains("filled_avg_price") && !obj.at("filled_avg_price").is_null())
        o.filled_avg_price = json::value_to<std::string>(obj.at("filled_avg_price"));
    if (obj.contains("limit_price") && !obj.at("limit_price").is_null())
        o.limit_price = json::value_to<std::string>(obj.at("limit_price"));
    if (obj.contains("stop_price") && !obj.at("stop_price").is_null())
        o.stop_price = json::value_to<std::string>(obj.at("stop_price"));
    if (obj.contains("trail_percent") && !obj.at("trail_percent").is_null())
        o.trail_percent = json::value_to<std::string>(obj.at("trail_percent"));
    if (obj.contains("trail_price") && !obj.at("trail_price").is_null())
        o.trail_price = json::value_to<std::string>(obj.at("trail_price"));
    if (obj.contains("hwm") && !obj.at("hwm").is_null())
        o.hwm = json::value_to<std::string>(obj.at("hwm"));
    if (obj.contains("position_intent") && !obj.at("position_intent").is_null())
        o.position_intent_type = json::value_to<position_intent>(obj.at("position_intent"));
    if (obj.contains("legs") && !obj.at("legs").is_null())
        o.legs = json::value_to<std::vector<order>>(obj.at("legs"));

    return o;
}

void tag_invoke(json::value_from_tag, json::value& jv, const order& o)
{
    json::object obj;
    obj["id"]              = o.id;
    obj["client_order_id"] = o.client_order_id;
    obj["created_at"]      = o.created_at;
    obj["asset_id"]        = o.asset_id;
    obj["symbol"]          = o.symbol;
    obj["asset_class"]     = json::value_from(o.asset_class_type);
    obj["filled_qty"]      = o.filled_qty;
    obj["order_class"]     = json::value_from(o.order_class_type);
    obj["order_type"]      = json::value_from(o.type);
    obj["side"]            = json::value_from(o.side);
    obj["time_in_force"]   = json::value_from(o.time_in_force_type);
    obj["status"]          = json::value_from(o.status);
    obj["extended_hours"]  = o.extended_hours;

    if (o.updated_at.has_value())
        obj["updated_at"] = o.updated_at.value();
    if (o.submitted_at.has_value())
        obj["submitted_at"] = o.submitted_at.value();
    if (o.filled_at.has_value())
        obj["filled_at"] = o.filled_at.value();
    if (o.expired_at.has_value())
        obj["expired_at"] = o.expired_at.value();
    if (o.canceled_at.has_value())
        obj["canceled_at"] = o.canceled_at.value();
    if (o.failed_at.has_value())
        obj["failed_at"] = o.failed_at.value();
    if (o.replaced_at.has_value())
        obj["replaced_at"] = o.replaced_at.value();
    if (o.replaced_by.has_value())
        obj["replaced_by"] = o.replaced_by.value();
    if (o.replaces.has_value())
        obj["replaces"] = o.replaces.value();
    if (o.notional.has_value())
        obj["notional"] = o.notional.value();
    if (o.qty.has_value())
        obj["qty"] = o.qty.value();
    if (o.filled_avg_price.has_value())
        obj["filled_avg_price"] = o.filled_avg_price.value();
    if (o.limit_price.has_value())
        obj["limit_price"] = o.limit_price.value();
    if (o.stop_price.has_value())
        obj["stop_price"] = o.stop_price.value();
    if (o.trail_percent.has_value())
        obj["trail_percent"] = o.trail_percent.value();
    if (o.trail_price.has_value())
        obj["trail_price"] = o.trail_price.value();
    if (o.hwm.has_value())
        obj["hwm"] = o.hwm.value();
    if (o.position_intent_type.has_value())
        obj["position_intent"] = json::value_from(o.position_intent_type.value());
    if (o.legs.has_value())
        obj["legs"] = json::value_from(o.legs.value());

    jv = std::move(obj);
}

void tag_invoke(json::value_from_tag, json::value& jv, const notional_order& order)
{
    json::object obj;
    obj["symbol"]         = order.symbol;
    obj["notional"]       = order.notional;
    obj["side"]           = json::value_from(order.side);
    obj["type"]           = "market";
    obj["time_in_force"]  = "day";
    obj["extended_hours"] = order.extended_hours;

    if (!order.client_order_id.empty())
        obj["client_order_id"] = order.client_order_id;

    jv = std::move(obj);
}

notional_order tag_invoke(json::value_to_tag<notional_order>, const json::value& jv)
{
    const auto&    obj = jv.as_object();
    notional_order order;

    order.symbol         = json::value_to<std::string>(obj.at("symbol"));
    order.notional       = json::value_to<std::string>(obj.at("notional"));
    order.side           = json::value_to<order_side>(obj.at("side"));
    order.extended_hours = json::value_to<bool>(obj.at("extended_hours"));

    if (obj.contains("client_order_id"))
        order.client_order_id = json::value_to<std::string>(obj.at("client_order_id"));

    return order;
}

position_closed tag_invoke(json::value_to_tag<position_closed>, const json::value& jv)
{
    const auto&     obj = jv.as_object();
    position_closed pc;

    pc.symbol        = json::value_to<std::string>(obj.at("symbol"));
    pc.status        = json::value_to<int>(obj.at("status"));
    pc.order_details = json::value_to<order>(obj.at("body"));

    return pc;
}

void tag_invoke(json::value_from_tag, json::value& jv, const position_closed& pc)
{
    json::object obj;
    obj["symbol"] = pc.symbol;
    obj["status"] = pc.status;
    obj["body"]   = json::value_from(pc.order_details);

    jv = std::move(obj);
}

order_deleted tag_invoke(json::value_to_tag<order_deleted>, const json::value& jv)
{
    const auto&   obj = jv.as_object();
    order_deleted od;

    od.id     = json::value_to<std::string>(obj.at("id"));
    od.status = json::value_to<int>(obj.at("status"));

    return od;
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_deleted& od)
{
    json::object obj;
    obj["id"]     = od.id;
    obj["status"] = od.status;

    jv = std::move(obj);
}
