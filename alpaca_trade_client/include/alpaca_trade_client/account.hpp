#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

enum class account_status
{
    ACCOUNT_CLOSED,
    ACCOUNT_UPDATED,
    ACTION_REQUIRED,
    ACTIVE,
    AML_REVIEW,
    APPROVAL_PENDING,
    APPROVED,
    DISABLED,
    DISABLE_PENDING,
    EDITED,
    INACTIVE,
    KYC_SUBMITTED,
    LIMITED,
    ONBOARDING,
    PAPER_ONLY,
    REAPPROVAL_PENDING,
    REJECTED,
    RESUBMITTED,
    SIGNED_UP,
    SUBMISSION_FAILED,
    SUBMITTED
};

struct trade_account
{
    std::string                   id{};
    std::string                   account_number{};
    account_status                status{};
    std::optional<account_status> crypto_status{};
    std::string                   currency{"USD"};
    std::optional<std::string>    buying_power{};
    std::optional<std::string>    regt_buying_power{};
    std::optional<std::string>    daytrading_buying_power{};
    std::optional<std::string>    non_marginable_buying_power{};
    std::optional<std::string>    cash{};
    std::optional<std::string>    accrued_fees{};
    std::optional<std::string>    pending_transfer_out{};
    std::optional<std::string>    pending_transfer_in{};
    std::optional<std::string>    portfolio_value{};
    std::optional<bool>           pattern_day_trader{};
    std::optional<bool>           trading_blocked{};
    std::optional<bool>           transfers_blocked{};
    std::optional<bool>           account_blocked{};
    std::optional<std::string>    created_at{};
    std::optional<bool>           trade_suspended_by_user{};
    std::optional<std::string>    multiplier{};
    std::optional<bool>           shorting_enabled{};
    std::optional<std::string>    equity{};
    std::optional<std::string>    last_equity{};
    std::optional<std::string>    long_market_value{};
    std::optional<std::string>    short_market_value{};
    std::optional<std::string>    initial_margin{};
    std::optional<std::string>    maintenance_margin{};
    std::optional<std::string>    last_maintenance_margin{};
    std::optional<std::string>    sma{};
    std::optional<int>            daytrade_count{};
    std::optional<std::string>    options_buying_power{};
    std::optional<int>            options_approved_level{};
    std::optional<int>            options_trading_level{};
};

inline void to_json(nlohmann::json& j, const account_status& status)
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

inline void from_json(const nlohmann::json& j, account_status& status)
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

inline void from_json(const nlohmann::json& j, trade_account& account)
{
    j.at("id").get_to(account.id);
    j.at("account_number").get_to(account.account_number);
    j.at("status").get_to(account.status);
    j.at("currency").get_to(account.currency);

    if (j.contains("crypto_status") && !j.at("crypto_status").is_null())
        account.crypto_status = j.at("crypto_status").get<account_status>();
    if (j.contains("buying_power") && !j.at("buying_power").is_null())
        account.buying_power = j.at("buying_power").get<std::string>();
    if (j.contains("regt_buying_power") && !j.at("regt_buying_power").is_null())
        account.regt_buying_power = j.at("regt_buying_power").get<std::string>();
    if (j.contains("daytrading_buying_power") && !j.at("daytrading_buying_power").is_null())
        account.daytrading_buying_power = j.at("daytrading_buying_power").get<std::string>();
    if (j.contains("non_marginable_buying_power") && !j.at("non_marginable_buying_power").is_null())
        account.non_marginable_buying_power = j.at("non_marginable_buying_power").get<std::string>();
    if (j.contains("cash") && !j.at("cash").is_null())
        account.cash = j.at("cash").get<std::string>();
    if (j.contains("accrued_fees") && !j.at("accrued_fees").is_null())
        account.accrued_fees = j.at("accrued_fees").get<std::string>();
    if (j.contains("pending_transfer_out") && !j.at("pending_transfer_out").is_null())
        account.pending_transfer_out = j.at("pending_transfer_out").get<std::string>();
    if (j.contains("pending_transfer_in") && !j.at("pending_transfer_in").is_null())
        account.pending_transfer_in = j.at("pending_transfer_in").get<std::string>();
    if (j.contains("portfolio_value") && !j.at("portfolio_value").is_null())
        account.portfolio_value = j.at("portfolio_value").get<std::string>();
    if (j.contains("pattern_day_trader") && !j.at("pattern_day_trader").is_null())
        account.pattern_day_trader = j.at("pattern_day_trader").get<bool>();
    if (j.contains("trading_blocked") && !j.at("trading_blocked").is_null())
        account.trading_blocked = j.at("trading_blocked").get<bool>();
    if (j.contains("transfers_blocked") && !j.at("transfers_blocked").is_null())
        account.transfers_blocked = j.at("transfers_blocked").get<bool>();
    if (j.contains("account_blocked") && !j.at("account_blocked").is_null())
        account.account_blocked = j.at("account_blocked").get<bool>();
    if (j.contains("created_at") && !j.at("created_at").is_null())
        account.created_at = j.at("created_at").get<std::string>();
    if (j.contains("trade_suspended_by_user") && !j.at("trade_suspended_by_user").is_null())
        account.trade_suspended_by_user = j.at("trade_suspended_by_user").get<bool>();
    if (j.contains("multiplier") && !j.at("multiplier").is_null())
        account.multiplier = j.at("multiplier").get<std::string>();
    if (j.contains("shorting_enabled") && !j.at("shorting_enabled").is_null())
        account.shorting_enabled = j.at("shorting_enabled").get<bool>();
    if (j.contains("equity") && !j.at("equity").is_null())
        account.equity = j.at("equity").get<std::string>();
    if (j.contains("last_equity") && !j.at("last_equity").is_null())
        account.last_equity = j.at("last_equity").get<std::string>();
    if (j.contains("long_market_value") && !j.at("long_market_value").is_null())
        account.long_market_value = j.at("long_market_value").get<std::string>();
    if (j.contains("short_market_value") && !j.at("short_market_value").is_null())
        account.short_market_value = j.at("short_market_value").get<std::string>();
    if (j.contains("initial_margin") && !j.at("initial_margin").is_null())
        account.initial_margin = j.at("initial_margin").get<std::string>();
    if (j.contains("maintenance_margin") && !j.at("maintenance_margin").is_null())
        account.maintenance_margin = j.at("maintenance_margin").get<std::string>();
    if (j.contains("last_maintenance_margin") && !j.at("last_maintenance_margin").is_null())
        account.last_maintenance_margin = j.at("last_maintenance_margin").get<std::string>();
    if (j.contains("sma") && !j.at("sma").is_null())
        account.sma = j.at("sma").get<std::string>();
    if (j.contains("daytrade_count") && !j.at("daytrade_count").is_null())
        account.daytrade_count = j.at("daytrade_count").get<int>();
    if (j.contains("options_buying_power") && !j.at("options_buying_power").is_null())
        account.options_buying_power = j.at("options_buying_power").get<std::string>();
    if (j.contains("options_approved_level") && !j.at("options_approved_level").is_null())
        account.options_approved_level = j.at("options_approved_level").get<int>();
    if (j.contains("options_trading_level") && !j.at("options_trading_level").is_null())
        account.options_trading_level = j.at("options_trading_level").get<int>();
}

inline void to_json(nlohmann::json& j, const trade_account& account)
{
    j = nlohmann::json{
        {"id", account.id},
        {"account_number", account.account_number},
        {"status", account.status},
        {"currency", account.currency}};

    if (account.crypto_status.has_value())
        j["crypto_status"] = account.crypto_status.value();
    if (account.buying_power.has_value())
        j["buying_power"] = account.buying_power.value();
    if (account.regt_buying_power.has_value())
        j["regt_buying_power"] = account.regt_buying_power.value();
    if (account.daytrading_buying_power.has_value())
        j["daytrading_buying_power"] = account.daytrading_buying_power.value();
    if (account.non_marginable_buying_power.has_value())
        j["non_marginable_buying_power"] = account.non_marginable_buying_power.value();
    if (account.cash.has_value())
        j["cash"] = account.cash.value();
    if (account.accrued_fees.has_value())
        j["accrued_fees"] = account.accrued_fees.value();
    if (account.pending_transfer_out.has_value())
        j["pending_transfer_out"] = account.pending_transfer_out.value();
    if (account.pending_transfer_in.has_value())
        j["pending_transfer_in"] = account.pending_transfer_in.value();
    if (account.portfolio_value.has_value())
        j["portfolio_value"] = account.portfolio_value.value();
    if (account.pattern_day_trader.has_value())
        j["pattern_day_trader"] = account.pattern_day_trader.value();
    if (account.trading_blocked.has_value())
        j["trading_blocked"] = account.trading_blocked.value();
    if (account.transfers_blocked.has_value())
        j["transfers_blocked"] = account.transfers_blocked.value();
    if (account.account_blocked.has_value())
        j["account_blocked"] = account.account_blocked.value();
    if (account.created_at.has_value())
        j["created_at"] = account.created_at.value();
    if (account.trade_suspended_by_user.has_value())
        j["trade_suspended_by_user"] = account.trade_suspended_by_user.value();
    if (account.multiplier.has_value())
        j["multiplier"] = account.multiplier.value();
    if (account.shorting_enabled.has_value())
        j["shorting_enabled"] = account.shorting_enabled.value();
    if (account.equity.has_value())
        j["equity"] = account.equity.value();
    if (account.last_equity.has_value())
        j["last_equity"] = account.last_equity.value();
    if (account.long_market_value.has_value())
        j["long_market_value"] = account.long_market_value.value();
    if (account.short_market_value.has_value())
        j["short_market_value"] = account.short_market_value.value();
    if (account.initial_margin.has_value())
        j["initial_margin"] = account.initial_margin.value();
    if (account.maintenance_margin.has_value())
        j["maintenance_margin"] = account.maintenance_margin.value();
    if (account.last_maintenance_margin.has_value())
        j["last_maintenance_margin"] = account.last_maintenance_margin.value();
    if (account.sma.has_value())
        j["sma"] = account.sma.value();
    if (account.daytrade_count.has_value())
        j["daytrade_count"] = account.daytrade_count.value();
    if (account.options_buying_power.has_value())
        j["options_buying_power"] = account.options_buying_power.value();
    if (account.options_approved_level.has_value())
        j["options_approved_level"] = account.options_approved_level.value();
    if (account.options_trading_level.has_value())
        j["options_trading_level"] = account.options_trading_level.value();
}
