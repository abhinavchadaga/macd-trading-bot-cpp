#pragma once

#include <boost/json.hpp>
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
    std::string                id{};
    std::string                currency{"USD"};
    account_status             status{};
    std::optional<std::string> account_number{};
    std::optional<std::string> cash{};
    std::optional<std::string> portfolio_value{};
    std::optional<std::string> non_marginable_buying_power{};
    std::optional<std::string> accrued_fees{};
    std::optional<std::string> pending_transfer_in{};
    std::optional<std::string> pending_transfer_out{};
    std::optional<std::string> created_at{};
    std::optional<std::string> long_market_value{};
    std::optional<std::string> short_market_value{};
    std::optional<std::string> equity{};
    std::optional<std::string> last_equity{};
    std::optional<std::string> multiplier{};
    std::optional<std::string> buying_power{};
    std::optional<std::string> initial_margin{};
    std::optional<std::string> maintenance_margin{};
    std::optional<std::string> sma{};
    std::optional<std::string> balance_asof{};
    std::optional<std::string> last_maintenance_margin{};
    std::optional<std::string> daytrading_buying_power{};
    std::optional<std::string> regt_buying_power{};
    std::optional<std::string> options_buying_power{};
    std::optional<std::string> intraday_adjustments{};
    std::optional<std::string> pending_reg_taf_fees{};
    std::optional<int>         daytrade_count{};
    std::optional<int>         options_approved_level{};
    std::optional<int>         options_trading_level{};
    std::optional<bool>        pattern_day_trader{};
    std::optional<bool>        trade_suspended_by_user{};
    std::optional<bool>        trading_blocked{};
    std::optional<bool>        transfers_blocked{};
    std::optional<bool>        account_blocked{};
    std::optional<bool>        shorting_enabled{};
};

void           tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const account_status& status);
account_status tag_invoke(boost::json::value_to_tag<account_status>, const boost::json::value& jv);

void          tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const trade_account& account);
trade_account tag_invoke(boost::json::value_to_tag<trade_account>, const boost::json::value& jv);
