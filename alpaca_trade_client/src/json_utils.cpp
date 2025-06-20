#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/orders.hpp"
#include "alpaca_trade_client/position.hpp"

//
// account.hpp JSON serialization implementations
//

namespace json = boost::json;

void tag_invoke(json::value_from_tag, json::value& jv, const account_status& status)
{
    switch (status)
    {
        case account_status::ACCOUNT_CLOSED:
            jv = "ACCOUNT_CLOSED";
            break;
        case account_status::ACCOUNT_UPDATED:
            jv = "ACCOUNT_UPDATED";
            break;
        case account_status::ACTION_REQUIRED:
            jv = "ACTION_REQUIRED";
            break;
        case account_status::ACTIVE:
            jv = "ACTIVE";
            break;
        case account_status::AML_REVIEW:
            jv = "AML_REVIEW";
            break;
        case account_status::APPROVAL_PENDING:
            jv = "APPROVAL_PENDING";
            break;
        case account_status::APPROVED:
            jv = "APPROVED";
            break;
        case account_status::DISABLED:
            jv = "DISABLED";
            break;
        case account_status::DISABLE_PENDING:
            jv = "DISABLE_PENDING";
            break;
        case account_status::EDITED:
            jv = "EDITED";
            break;
        case account_status::INACTIVE:
            jv = "INACTIVE";
            break;
        case account_status::KYC_SUBMITTED:
            jv = "KYC_SUBMITTED";
            break;
        case account_status::LIMITED:
            jv = "LIMITED";
            break;
        case account_status::ONBOARDING:
            jv = "ONBOARDING";
            break;
        case account_status::PAPER_ONLY:
            jv = "PAPER_ONLY";
            break;
        case account_status::REAPPROVAL_PENDING:
            jv = "REAPPROVAL_PENDING";
            break;
        case account_status::REJECTED:
            jv = "REJECTED";
            break;
        case account_status::RESUBMITTED:
            jv = "RESUBMITTED";
            break;
        case account_status::SIGNED_UP:
            jv = "SIGNED_UP";
            break;
        case account_status::SUBMISSION_FAILED:
            jv = "SUBMISSION_FAILED";
            break;
        case account_status::SUBMITTED:
            jv = "SUBMITTED";
            break;
    }
}

account_status tag_invoke(json::value_to_tag<account_status>, const json::value& jv)
{
    const std::string status_str = json::value_to<std::string>(jv);
    if (status_str == "ACCOUNT_CLOSED")
        return account_status::ACCOUNT_CLOSED;
    if (status_str == "ACCOUNT_UPDATED")
        return account_status::ACCOUNT_UPDATED;
    if (status_str == "ACTION_REQUIRED")
        return account_status::ACTION_REQUIRED;
    if (status_str == "ACTIVE")
        return account_status::ACTIVE;
    if (status_str == "AML_REVIEW")
        return account_status::AML_REVIEW;
    if (status_str == "APPROVAL_PENDING")
        return account_status::APPROVAL_PENDING;
    if (status_str == "APPROVED")
        return account_status::APPROVED;
    if (status_str == "DISABLED")
        return account_status::DISABLED;
    if (status_str == "DISABLE_PENDING")
        return account_status::DISABLE_PENDING;
    if (status_str == "EDITED")
        return account_status::EDITED;
    if (status_str == "INACTIVE")
        return account_status::INACTIVE;
    if (status_str == "KYC_SUBMITTED")
        return account_status::KYC_SUBMITTED;
    if (status_str == "LIMITED")
        return account_status::LIMITED;
    if (status_str == "ONBOARDING")
        return account_status::ONBOARDING;
    if (status_str == "PAPER_ONLY")
        return account_status::PAPER_ONLY;
    if (status_str == "REAPPROVAL_PENDING")
        return account_status::REAPPROVAL_PENDING;
    if (status_str == "REJECTED")
        return account_status::REJECTED;
    if (status_str == "RESUBMITTED")
        return account_status::RESUBMITTED;
    if (status_str == "SIGNED_UP")
        return account_status::SIGNED_UP;
    if (status_str == "SUBMISSION_FAILED")
        return account_status::SUBMISSION_FAILED;
    if (status_str == "SUBMITTED")
        return account_status::SUBMITTED;
    throw std::invalid_argument("Invalid account_status: " + status_str);
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
    switch (exchange)
    {
        case asset_exchange::AMEX:
            jv = "AMEX";
            break;
        case asset_exchange::ARCA:
            jv = "ARCA";
            break;
        case asset_exchange::BATS:
            jv = "BATS";
            break;
        case asset_exchange::NYSE:
            jv = "NYSE";
            break;
        case asset_exchange::NASDAQ:
            jv = "NASDAQ";
            break;
        case asset_exchange::NYSEARCA:
            jv = "NYSEARCA";
            break;
        case asset_exchange::OTC:
            jv = "OTC";
            break;
        case asset_exchange::FTXU:
            jv = "FTXU";
            break;
        case asset_exchange::CBSE:
            jv = "CBSE";
            break;
        case asset_exchange::GNSS:
            jv = "GNSS";
            break;
        case asset_exchange::ERSX:
            jv = "ERSX";
            break;
        case asset_exchange::CRYPTO:
            jv = "CRYPTO";
            break;
        case asset_exchange::EMPTY:
            jv = "";
            break;
    }
}

asset_exchange tag_invoke(json::value_to_tag<asset_exchange>, const json::value& jv)
{
    const std::string exchange_str = json::value_to<std::string>(jv);
    if (exchange_str == "AMEX")
        return asset_exchange::AMEX;
    if (exchange_str == "ARCA")
        return asset_exchange::ARCA;
    if (exchange_str == "BATS")
        return asset_exchange::BATS;
    if (exchange_str == "NYSE")
        return asset_exchange::NYSE;
    if (exchange_str == "NASDAQ")
        return asset_exchange::NASDAQ;
    if (exchange_str == "NYSEARCA")
        return asset_exchange::NYSEARCA;
    if (exchange_str == "OTC")
        return asset_exchange::OTC;
    if (exchange_str == "FTXU")
        return asset_exchange::FTXU;
    if (exchange_str == "CBSE")
        return asset_exchange::CBSE;
    if (exchange_str == "GNSS")
        return asset_exchange::GNSS;
    if (exchange_str == "ERSX")
        return asset_exchange::ERSX;
    if (exchange_str == "CRYPTO")
        return asset_exchange::CRYPTO;
    if (exchange_str == "")
        return asset_exchange::EMPTY;
    throw std::invalid_argument("Invalid asset_exchange: " + exchange_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const asset_class& asset_class_type)
{
    switch (asset_class_type)
    {
        case asset_class::US_EQUITY:
            jv = "us_equity";
            break;
        case asset_class::US_OPTION:
            jv = "us_option";
            break;
        case asset_class::CRYPTO:
            jv = "crypto";
            break;
    }
}

asset_class tag_invoke(json::value_to_tag<asset_class>, const json::value& jv)
{
    const std::string class_str = json::value_to<std::string>(jv);
    if (class_str == "us_equity")
        return asset_class::US_EQUITY;
    if (class_str == "us_option")
        return asset_class::US_OPTION;
    if (class_str == "crypto")
        return asset_class::CRYPTO;
    throw std::invalid_argument("Invalid asset_class: " + class_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const position_side& side)
{
    switch (side)
    {
        case position_side::LONG:
            jv = "long";
            break;
        case position_side::SHORT:
            jv = "short";
            break;
    }
}

position_side tag_invoke(json::value_to_tag<position_side>, const json::value& jv)
{
    const std::string side_str = json::value_to<std::string>(jv);
    if (side_str == "long")
        return position_side::LONG;
    if (side_str == "short")
        return position_side::SHORT;
    throw std::invalid_argument("Invalid position_side: " + side_str);
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
    switch (side)
    {
        case order_side::BUY:
            jv = "buy";
            break;
        case order_side::SELL:
            jv = "sell";
            break;
    }
}

order_side tag_invoke(json::value_to_tag<order_side>, const json::value& jv)
{
    const std::string side_str = json::value_to<std::string>(jv);
    if (side_str == "buy")
        return order_side::BUY;
    if (side_str == "sell")
        return order_side::SELL;
    throw std::invalid_argument("Invalid order_side: " + side_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_class& oc)
{
    switch (oc)
    {
        case order_class::SIMPLE:
            jv = "simple";
            break;
        case order_class::BRACKET:
            jv = "bracket";
            break;
        case order_class::OCO:
            jv = "oco";
            break;
        case order_class::OTO:
            jv = "oto";
            break;
        case order_class::MLEG:
            jv = "mleg";
            break;
    }
}

order_class tag_invoke(json::value_to_tag<order_class>, const json::value& jv)
{
    const std::string class_str = json::value_to<std::string>(jv);
    if (class_str == "simple" || class_str.empty())
        return order_class::SIMPLE;
    if (class_str == "bracket")
        return order_class::BRACKET;
    if (class_str == "oco")
        return order_class::OCO;
    if (class_str == "oto")
        return order_class::OTO;
    if (class_str == "mleg")
        return order_class::MLEG;
    throw std::invalid_argument("Invalid order_class: " + class_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_type& ot)
{
    switch (ot)
    {
        case order_type::MARKET:
            jv = "market";
            break;
        case order_type::LIMIT:
            jv = "limit";
            break;
        case order_type::STOP:
            jv = "stop";
            break;
        case order_type::STOP_LIMIT:
            jv = "stop_limit";
            break;
        case order_type::TRAILING_STOP:
            jv = "trailing_stop";
            break;
    }
}

order_type tag_invoke(json::value_to_tag<order_type>, const json::value& jv)
{
    const std::string type_str = json::value_to<std::string>(jv);
    if (type_str == "market")
        return order_type::MARKET;
    if (type_str == "limit")
        return order_type::LIMIT;
    if (type_str == "stop")
        return order_type::STOP;
    if (type_str == "stop_limit")
        return order_type::STOP_LIMIT;
    if (type_str == "trailing_stop")
        return order_type::TRAILING_STOP;
    throw std::invalid_argument("Invalid order_type: " + type_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const time_in_force& tif)
{
    switch (tif)
    {
        case time_in_force::DAY:
            jv = "day";
            break;
        case time_in_force::GTC:
            jv = "gtc";
            break;
        case time_in_force::OPG:
            jv = "opg";
            break;
        case time_in_force::CLS:
            jv = "cls";
            break;
        case time_in_force::IOC:
            jv = "ioc";
            break;
        case time_in_force::FOK:
            jv = "fok";
            break;
    }
}

time_in_force tag_invoke(json::value_to_tag<time_in_force>, const json::value& jv)
{
    const std::string tif_str = json::value_to<std::string>(jv);
    if (tif_str == "day")
        return time_in_force::DAY;
    if (tif_str == "gtc")
        return time_in_force::GTC;
    if (tif_str == "opg")
        return time_in_force::OPG;
    if (tif_str == "cls")
        return time_in_force::CLS;
    if (tif_str == "ioc")
        return time_in_force::IOC;
    if (tif_str == "fok")
        return time_in_force::FOK;
    throw std::invalid_argument("Invalid time_in_force: " + tif_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const order_status& os)
{
    switch (os)
    {
        case order_status::NEW:
            jv = "new";
            break;
        case order_status::PARTIALLY_FILLED:
            jv = "partially_filled";
            break;
        case order_status::FILLED:
            jv = "filled";
            break;
        case order_status::DONE_FOR_DAY:
            jv = "done_for_day";
            break;
        case order_status::CANCELED:
            jv = "canceled";
            break;
        case order_status::EXPIRED:
            jv = "expired";
            break;
        case order_status::REPLACED:
            jv = "replaced";
            break;
        case order_status::PENDING_CANCEL:
            jv = "pending_cancel";
            break;
        case order_status::PENDING_REPLACE:
            jv = "pending_replace";
            break;
        case order_status::ACCEPTED:
            jv = "accepted";
            break;
        case order_status::PENDING_NEW:
            jv = "pending_new";
            break;
        case order_status::ACCEPTED_FOR_BIDDING:
            jv = "accepted_for_bidding";
            break;
        case order_status::STOPPED:
            jv = "stopped";
            break;
        case order_status::REJECTED:
            jv = "rejected";
            break;
        case order_status::SUSPENDED:
            jv = "suspended";
            break;
        case order_status::CALCULATED:
            jv = "calculated";
            break;
    }
}

order_status tag_invoke(json::value_to_tag<order_status>, const json::value& jv)
{
    const std::string status_str = json::value_to<std::string>(jv);
    if (status_str == "new")
        return order_status::NEW;
    if (status_str == "partially_filled")
        return order_status::PARTIALLY_FILLED;
    if (status_str == "filled")
        return order_status::FILLED;
    if (status_str == "done_for_day")
        return order_status::DONE_FOR_DAY;
    if (status_str == "canceled")
        return order_status::CANCELED;
    if (status_str == "expired")
        return order_status::EXPIRED;
    if (status_str == "replaced")
        return order_status::REPLACED;
    if (status_str == "pending_cancel")
        return order_status::PENDING_CANCEL;
    if (status_str == "pending_replace")
        return order_status::PENDING_REPLACE;
    if (status_str == "accepted")
        return order_status::ACCEPTED;
    if (status_str == "pending_new")
        return order_status::PENDING_NEW;
    if (status_str == "accepted_for_bidding")
        return order_status::ACCEPTED_FOR_BIDDING;
    if (status_str == "stopped")
        return order_status::STOPPED;
    if (status_str == "rejected")
        return order_status::REJECTED;
    if (status_str == "suspended")
        return order_status::SUSPENDED;
    if (status_str == "calculated")
        return order_status::CALCULATED;
    throw std::invalid_argument("Invalid order_status: " + status_str);
}

void tag_invoke(json::value_from_tag, json::value& jv, const position_intent& pi)
{
    switch (pi)
    {
        case position_intent::BUY_TO_OPEN:
            jv = "buy_to_open";
            break;
        case position_intent::BUY_TO_CLOSE:
            jv = "buy_to_close";
            break;
        case position_intent::SELL_TO_OPEN:
            jv = "sell_to_open";
            break;
        case position_intent::SELL_TO_CLOSE:
            jv = "sell_to_close";
            break;
    }
}

position_intent tag_invoke(json::value_to_tag<position_intent>, const json::value& jv)
{
    const std::string intent_str = json::value_to<std::string>(jv);
    if (intent_str == "buy_to_open")
        return position_intent::BUY_TO_OPEN;
    if (intent_str == "buy_to_close")
        return position_intent::BUY_TO_CLOSE;
    if (intent_str == "sell_to_open")
        return position_intent::SELL_TO_OPEN;
    if (intent_str == "sell_to_close")
        return position_intent::SELL_TO_CLOSE;
    throw std::invalid_argument("Invalid position_intent: " + intent_str);
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
