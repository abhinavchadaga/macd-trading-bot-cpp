#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/orders.hpp"
#include "alpaca_trade_client/position.hpp"

//
// account.hpp JSON serialization implementations

void to_json(nlohmann::json& j, const account_status& status)
{
    switch (status)
    {
        case account_status::ACCOUNT_CLOSED:
            j = "ACCOUNT_CLOSED";
            break;
        case account_status::ACCOUNT_UPDATED:
            j = "ACCOUNT_UPDATED";
            break;
        case account_status::ACTION_REQUIRED:
            j = "ACTION_REQUIRED";
            break;
        case account_status::ACTIVE:
            j = "ACTIVE";
            break;
        case account_status::AML_REVIEW:
            j = "AML_REVIEW";
            break;
        case account_status::APPROVAL_PENDING:
            j = "APPROVAL_PENDING";
            break;
        case account_status::APPROVED:
            j = "APPROVED";
            break;
        case account_status::DISABLED:
            j = "DISABLED";
            break;
        case account_status::DISABLE_PENDING:
            j = "DISABLE_PENDING";
            break;
        case account_status::EDITED:
            j = "EDITED";
            break;
        case account_status::INACTIVE:
            j = "INACTIVE";
            break;
        case account_status::KYC_SUBMITTED:
            j = "KYC_SUBMITTED";
            break;
        case account_status::LIMITED:
            j = "LIMITED";
            break;
        case account_status::ONBOARDING:
            j = "ONBOARDING";
            break;
        case account_status::PAPER_ONLY:
            j = "PAPER_ONLY";
            break;
        case account_status::REAPPROVAL_PENDING:
            j = "REAPPROVAL_PENDING";
            break;
        case account_status::REJECTED:
            j = "REJECTED";
            break;
        case account_status::RESUBMITTED:
            j = "RESUBMITTED";
            break;
        case account_status::SIGNED_UP:
            j = "SIGNED_UP";
            break;
        case account_status::SUBMISSION_FAILED:
            j = "SUBMISSION_FAILED";
            break;
        case account_status::SUBMITTED:
            j = "SUBMITTED";
            break;
    }
}

void from_json(const nlohmann::json& j, account_status& status)
{
    const std::string status_str = j.get<std::string>();
    if (status_str == "ACCOUNT_CLOSED")
        status = account_status::ACCOUNT_CLOSED;
    else if (status_str == "ACCOUNT_UPDATED")
        status = account_status::ACCOUNT_UPDATED;
    else if (status_str == "ACTION_REQUIRED")
        status = account_status::ACTION_REQUIRED;
    else if (status_str == "ACTIVE")
        status = account_status::ACTIVE;
    else if (status_str == "AML_REVIEW")
        status = account_status::AML_REVIEW;
    else if (status_str == "APPROVAL_PENDING")
        status = account_status::APPROVAL_PENDING;
    else if (status_str == "APPROVED")
        status = account_status::APPROVED;
    else if (status_str == "DISABLED")
        status = account_status::DISABLED;
    else if (status_str == "DISABLE_PENDING")
        status = account_status::DISABLE_PENDING;
    else if (status_str == "EDITED")
        status = account_status::EDITED;
    else if (status_str == "INACTIVE")
        status = account_status::INACTIVE;
    else if (status_str == "KYC_SUBMITTED")
        status = account_status::KYC_SUBMITTED;
    else if (status_str == "LIMITED")
        status = account_status::LIMITED;
    else if (status_str == "ONBOARDING")
        status = account_status::ONBOARDING;
    else if (status_str == "PAPER_ONLY")
        status = account_status::PAPER_ONLY;
    else if (status_str == "REAPPROVAL_PENDING")
        status = account_status::REAPPROVAL_PENDING;
    else if (status_str == "REJECTED")
        status = account_status::REJECTED;
    else if (status_str == "RESUBMITTED")
        status = account_status::RESUBMITTED;
    else if (status_str == "SIGNED_UP")
        status = account_status::SIGNED_UP;
    else if (status_str == "SUBMISSION_FAILED")
        status = account_status::SUBMISSION_FAILED;
    else if (status_str == "SUBMITTED")
        status = account_status::SUBMITTED;
    else
        throw std::invalid_argument("Invalid account_status: " + status_str);
}

void from_json(const nlohmann::json& j, trade_account& account)
{
    j.at("id").get_to(account.id);
    j.at("status").get_to(account.status);
    j.at("currency").get_to(account.currency);

    if (j.contains("account_number") && !j.at("account_number").is_null())
        account.account_number = j.at("account_number").get<std::string>();
    if (j.contains("cash") && !j.at("cash").is_null())
        account.cash = j.at("cash").get<std::string>();
    if (j.contains("portfolio_value") && !j.at("portfolio_value").is_null())
        account.portfolio_value = j.at("portfolio_value").get<std::string>();
    if (j.contains("non_marginable_buying_power") && !j.at("non_marginable_buying_power").is_null())
        account.non_marginable_buying_power = j.at("non_marginable_buying_power").get<std::string>();
    if (j.contains("accrued_fees") && !j.at("accrued_fees").is_null())
        account.accrued_fees = j.at("accrued_fees").get<std::string>();
    if (j.contains("pending_transfer_in") && !j.at("pending_transfer_in").is_null())
        account.pending_transfer_in = j.at("pending_transfer_in").get<std::string>();
    if (j.contains("pending_transfer_out") && !j.at("pending_transfer_out").is_null())
        account.pending_transfer_out = j.at("pending_transfer_out").get<std::string>();
    if (j.contains("created_at") && !j.at("created_at").is_null())
        account.created_at = j.at("created_at").get<std::string>();
    if (j.contains("long_market_value") && !j.at("long_market_value").is_null())
        account.long_market_value = j.at("long_market_value").get<std::string>();
    if (j.contains("short_market_value") && !j.at("short_market_value").is_null())
        account.short_market_value = j.at("short_market_value").get<std::string>();
    if (j.contains("equity") && !j.at("equity").is_null())
        account.equity = j.at("equity").get<std::string>();
    if (j.contains("last_equity") && !j.at("last_equity").is_null())
        account.last_equity = j.at("last_equity").get<std::string>();
    if (j.contains("multiplier") && !j.at("multiplier").is_null())
        account.multiplier = j.at("multiplier").get<std::string>();
    if (j.contains("buying_power") && !j.at("buying_power").is_null())
        account.buying_power = j.at("buying_power").get<std::string>();
    if (j.contains("initial_margin") && !j.at("initial_margin").is_null())
        account.initial_margin = j.at("initial_margin").get<std::string>();
    if (j.contains("maintenance_margin") && !j.at("maintenance_margin").is_null())
        account.maintenance_margin = j.at("maintenance_margin").get<std::string>();
    if (j.contains("sma") && !j.at("sma").is_null())
        account.sma = j.at("sma").get<std::string>();
    if (j.contains("balance_asof") && !j.at("balance_asof").is_null())
        account.balance_asof = j.at("balance_asof").get<std::string>();
    if (j.contains("last_maintenance_margin") && !j.at("last_maintenance_margin").is_null())
        account.last_maintenance_margin = j.at("last_maintenance_margin").get<std::string>();
    if (j.contains("daytrading_buying_power") && !j.at("daytrading_buying_power").is_null())
        account.daytrading_buying_power = j.at("daytrading_buying_power").get<std::string>();
    if (j.contains("regt_buying_power") && !j.at("regt_buying_power").is_null())
        account.regt_buying_power = j.at("regt_buying_power").get<std::string>();
    if (j.contains("options_buying_power") && !j.at("options_buying_power").is_null())
        account.options_buying_power = j.at("options_buying_power").get<std::string>();
    if (j.contains("intraday_adjustments") && !j.at("intraday_adjustments").is_null())
        account.intraday_adjustments = j.at("intraday_adjustments").get<std::string>();
    if (j.contains("pending_reg_taf_fees") && !j.at("pending_reg_taf_fees").is_null())
        account.pending_reg_taf_fees = j.at("pending_reg_taf_fees").get<std::string>();
    if (j.contains("daytrade_count") && !j.at("daytrade_count").is_null())
        account.daytrade_count = j.at("daytrade_count").get<int>();
    if (j.contains("options_approved_level") && !j.at("options_approved_level").is_null())
        account.options_approved_level = j.at("options_approved_level").get<int>();
    if (j.contains("options_trading_level") && !j.at("options_trading_level").is_null())
        account.options_trading_level = j.at("options_trading_level").get<int>();
    if (j.contains("pattern_day_trader") && !j.at("pattern_day_trader").is_null())
        account.pattern_day_trader = j.at("pattern_day_trader").get<bool>();
    if (j.contains("trade_suspended_by_user") && !j.at("trade_suspended_by_user").is_null())
        account.trade_suspended_by_user = j.at("trade_suspended_by_user").get<bool>();
    if (j.contains("trading_blocked") && !j.at("trading_blocked").is_null())
        account.trading_blocked = j.at("trading_blocked").get<bool>();
    if (j.contains("transfers_blocked") && !j.at("transfers_blocked").is_null())
        account.transfers_blocked = j.at("transfers_blocked").get<bool>();
    if (j.contains("account_blocked") && !j.at("account_blocked").is_null())
        account.account_blocked = j.at("account_blocked").get<bool>();
    if (j.contains("shorting_enabled") && !j.at("shorting_enabled").is_null())
        account.shorting_enabled = j.at("shorting_enabled").get<bool>();
}

void to_json(nlohmann::json& j, const trade_account& account)
{
    j = nlohmann::json{{"id", account.id}, {"currency", account.currency}, {"status", account.status}};

    if (account.account_number.has_value())
        j["account_number"] = account.account_number.value();
    if (account.cash.has_value())
        j["cash"] = account.cash.value();
    if (account.portfolio_value.has_value())
        j["portfolio_value"] = account.portfolio_value.value();
    if (account.non_marginable_buying_power.has_value())
        j["non_marginable_buying_power"] = account.non_marginable_buying_power.value();
    if (account.accrued_fees.has_value())
        j["accrued_fees"] = account.accrued_fees.value();
    if (account.pending_transfer_in.has_value())
        j["pending_transfer_in"] = account.pending_transfer_in.value();
    if (account.pending_transfer_out.has_value())
        j["pending_transfer_out"] = account.pending_transfer_out.value();
    if (account.created_at.has_value())
        j["created_at"] = account.created_at.value();
    if (account.long_market_value.has_value())
        j["long_market_value"] = account.long_market_value.value();
    if (account.short_market_value.has_value())
        j["short_market_value"] = account.short_market_value.value();
    if (account.equity.has_value())
        j["equity"] = account.equity.value();
    if (account.last_equity.has_value())
        j["last_equity"] = account.last_equity.value();
    if (account.multiplier.has_value())
        j["multiplier"] = account.multiplier.value();
    if (account.buying_power.has_value())
        j["buying_power"] = account.buying_power.value();
    if (account.initial_margin.has_value())
        j["initial_margin"] = account.initial_margin.value();
    if (account.maintenance_margin.has_value())
        j["maintenance_margin"] = account.maintenance_margin.value();
    if (account.sma.has_value())
        j["sma"] = account.sma.value();
    if (account.balance_asof.has_value())
        j["balance_asof"] = account.balance_asof.value();
    if (account.last_maintenance_margin.has_value())
        j["last_maintenance_margin"] = account.last_maintenance_margin.value();
    if (account.daytrading_buying_power.has_value())
        j["daytrading_buying_power"] = account.daytrading_buying_power.value();
    if (account.regt_buying_power.has_value())
        j["regt_buying_power"] = account.regt_buying_power.value();
    if (account.options_buying_power.has_value())
        j["options_buying_power"] = account.options_buying_power.value();
    if (account.intraday_adjustments.has_value())
        j["intraday_adjustments"] = account.intraday_adjustments.value();
    if (account.pending_reg_taf_fees.has_value())
        j["pending_reg_taf_fees"] = account.pending_reg_taf_fees.value();
    if (account.daytrade_count.has_value())
        j["daytrade_count"] = account.daytrade_count.value();
    if (account.options_approved_level.has_value())
        j["options_approved_level"] = account.options_approved_level.value();
    if (account.options_trading_level.has_value())
        j["options_trading_level"] = account.options_trading_level.value();
    if (account.pattern_day_trader.has_value())
        j["pattern_day_trader"] = account.pattern_day_trader.value();
    if (account.trade_suspended_by_user.has_value())
        j["trade_suspended_by_user"] = account.trade_suspended_by_user.value();
    if (account.trading_blocked.has_value())
        j["trading_blocked"] = account.trading_blocked.value();
    if (account.transfers_blocked.has_value())
        j["transfers_blocked"] = account.transfers_blocked.value();
    if (account.account_blocked.has_value())
        j["account_blocked"] = account.account_blocked.value();
    if (account.shorting_enabled.has_value())
        j["shorting_enabled"] = account.shorting_enabled.value();
}

//
// position.hpp JSON serialization implementations

void to_json(nlohmann::json& j, const asset_exchange& exchange)
{
    switch (exchange)
    {
        case asset_exchange::AMEX:
            j = "AMEX";
            break;
        case asset_exchange::ARCA:
            j = "ARCA";
            break;
        case asset_exchange::BATS:
            j = "BATS";
            break;
        case asset_exchange::NYSE:
            j = "NYSE";
            break;
        case asset_exchange::NASDAQ:
            j = "NASDAQ";
            break;
        case asset_exchange::NYSEARCA:
            j = "NYSEARCA";
            break;
        case asset_exchange::OTC:
            j = "OTC";
            break;
        case asset_exchange::FTXU:
            j = "FTXU";
            break;
        case asset_exchange::CBSE:
            j = "CBSE";
            break;
        case asset_exchange::GNSS:
            j = "GNSS";
            break;
        case asset_exchange::ERSX:
            j = "ERSX";
            break;
        case asset_exchange::CRYPTO:
            j = "CRYPTO";
            break;
        case asset_exchange::EMPTY:
            j = "";
            break;
    }
}

void from_json(const nlohmann::json& j, asset_exchange& exchange)
{
    if (const std::string exchange_str = j; exchange_str == "AMEX")
        exchange = asset_exchange::AMEX;
    else if (exchange_str == "ARCA")
        exchange = asset_exchange::ARCA;
    else if (exchange_str == "BATS")
        exchange = asset_exchange::BATS;
    else if (exchange_str == "NYSE")
        exchange = asset_exchange::NYSE;
    else if (exchange_str == "NASDAQ")
        exchange = asset_exchange::NASDAQ;
    else if (exchange_str == "NYSEARCA")
        exchange = asset_exchange::NYSEARCA;
    else if (exchange_str == "OTC")
        exchange = asset_exchange::OTC;
    else if (exchange_str == "FTXU")
        exchange = asset_exchange::FTXU;
    else if (exchange_str == "CBSE")
        exchange = asset_exchange::CBSE;
    else if (exchange_str == "GNSS")
        exchange = asset_exchange::GNSS;
    else if (exchange_str == "ERSX")
        exchange = asset_exchange::ERSX;
    else if (exchange_str == "CRYPTO")
        exchange = asset_exchange::CRYPTO;
    else if (exchange_str == "")
        exchange = asset_exchange::EMPTY;
    else
        throw std::invalid_argument("Invalid asset_exchange: " + exchange_str);
}

void to_json(nlohmann::json& j, const asset_class& asset_class_type)
{
    switch (asset_class_type)
    {
        case asset_class::US_EQUITY:
            j = "us_equity";
            break;
        case asset_class::US_OPTION:
            j = "us_option";
            break;
        case asset_class::CRYPTO:
            j = "crypto";
            break;
    }
}

void from_json(const nlohmann::json& j, asset_class& asset_class_type)
{
    if (const std::string class_str = j; class_str == "us_equity")
        asset_class_type = asset_class::US_EQUITY;
    else if (class_str == "us_option")
        asset_class_type = asset_class::US_OPTION;
    else if (class_str == "crypto")
        asset_class_type = asset_class::CRYPTO;
    else
        throw std::invalid_argument("Invalid asset_class: " + class_str);
}

void to_json(nlohmann::json& j, const position_side& side)
{
    switch (side)
    {
        case position_side::LONG:
            j = "long";
            break;
        case position_side::SHORT:
            j = "short";
            break;
    }
}

void from_json(const nlohmann::json& j, position_side& side)
{
    if (const std::string side_str = j; side_str == "long")
        side = position_side::LONG;
    else if (side_str == "short")
        side = position_side::SHORT;
    else
        throw std::invalid_argument("Invalid position_side: " + side_str);
}

void from_json(const nlohmann::json& j, position& pos)
{
    j.at("asset_id").get_to(pos.asset_id);
    j.at("symbol").get_to(pos.symbol);
    j.at("exchange").get_to(pos.exchange);
    j.at("asset_class").get_to(pos.asset_class);
    j.at("avg_entry_price").get_to(pos.avg_entry_price);
    j.at("qty").get_to(pos.qty);
    j.at("side").get_to(pos.side);
    j.at("market_value").get_to(pos.market_value);
    j.at("cost_basis").get_to(pos.cost_basis);
    j.at("unrealized_pl").get_to(pos.unrealized_pl);
    j.at("unrealized_plpc").get_to(pos.unrealized_plpc);
    j.at("unrealized_intraday_pl").get_to(pos.unrealized_intraday_pl);
    j.at("unrealized_intraday_plpc").get_to(pos.unrealized_intraday_plpc);
    j.at("current_price").get_to(pos.current_price);
    j.at("lastday_price").get_to(pos.lastday_price);
    j.at("change_today").get_to(pos.change_today);
    j.at("asset_marginable").get_to(pos.asset_marginable);

    if (j.contains("qty_available") && !j.at("qty_available").is_null())
        pos.qty_available = j.at("qty_available").get<std::string>();
}

void to_json(nlohmann::json& j, const position& pos)
{
    j = nlohmann::json{
        {"asset_id", pos.asset_id},
        {"symbol", pos.symbol},
        {"exchange", pos.exchange},
        {"asset_class", pos.asset_class},
        {"avg_entry_price", pos.avg_entry_price},
        {"qty", pos.qty},
        {"side", pos.side},
        {"market_value", pos.market_value},
        {"cost_basis", pos.cost_basis},
        {"unrealized_pl", pos.unrealized_pl},
        {"unrealized_plpc", pos.unrealized_plpc},
        {"unrealized_intraday_pl", pos.unrealized_intraday_pl},
        {"unrealized_intraday_plpc", pos.unrealized_intraday_plpc},
        {"current_price", pos.current_price},
        {"lastday_price", pos.lastday_price},
        {"change_today", pos.change_today},
        {"asset_marginable", pos.asset_marginable}};

    if (pos.qty_available.has_value())
        j["qty_available"] = pos.qty_available.value();
}

//
// orders.hpp JSON serialization implementations

void to_json(nlohmann::json& j, const order_side& side)
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

void from_json(const nlohmann::json& j, order_side& side)
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

void to_json(nlohmann::json& j, const order_class& oc)
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

void from_json(const nlohmann::json& j, order_class& oc)
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

void to_json(nlohmann::json& j, const order_type& ot)
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

void from_json(const nlohmann::json& j, order_type& ot)
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

void to_json(nlohmann::json& j, const time_in_force& tif)
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

void from_json(const nlohmann::json& j, time_in_force& tif)
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

void to_json(nlohmann::json& j, const order_status& os)
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

void from_json(const nlohmann::json& j, order_status& os)
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

void to_json(nlohmann::json& j, const position_intent& pi)
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

void from_json(const nlohmann::json& j, position_intent& pi)
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

void from_json(const nlohmann::json& j, order& o)
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

void to_json(nlohmann::json& j, const order& o)
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

void to_json(nlohmann::json& j, const notional_order& order)
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

void from_json(const nlohmann::json& j, notional_order& order)
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

void from_json(const nlohmann::json& j, position_closed& pc)
{
    j.at("symbol").get_to(pc.symbol);
    j.at("status").get_to(pc.status);
    j.at("body").get_to(pc.order);
}

void to_json(nlohmann::json& j, const position_closed& pc)
{
    j = nlohmann::json{{"symbol", pc.symbol}, {"status", pc.status}, {"body", pc.order}};
}

void from_json(const nlohmann::json& j, order_deleted& od)
{
    j.at("id").get_to(od.id);
    j.at("status").get_to(od.status);
}

void to_json(nlohmann::json& j, const order_deleted& od)
{
    j = nlohmann::json{{"id", od.id}, {"status", od.status}};
}
