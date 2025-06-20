#pragma once

#include <algorithm>
#include <array>
#include <boost/json.hpp>
#include <stdexcept>
#include <string_view>

#include "account.hpp"
#include "orders.hpp"
#include "position.hpp"

namespace json = boost::json;

template<typename EnumType>
struct enum_mapping
{
    static constexpr std::array<std::pair<EnumType, std::string_view>, 0> mappings{};
};

template<>
struct enum_mapping<account_status>
{
    static constexpr std::array<std::pair<account_status, std::string_view>, 21> mappings{
        {{account_status::ACCOUNT_CLOSED, "ACCOUNT_CLOSED"},
         {account_status::ACCOUNT_UPDATED, "ACCOUNT_UPDATED"},
         {account_status::ACTION_REQUIRED, "ACTION_REQUIRED"},
         {account_status::ACTIVE, "ACTIVE"},
         {account_status::AML_REVIEW, "AML_REVIEW"},
         {account_status::APPROVAL_PENDING, "APPROVAL_PENDING"},
         {account_status::APPROVED, "APPROVED"},
         {account_status::DISABLED, "DISABLED"},
         {account_status::DISABLE_PENDING, "DISABLE_PENDING"},
         {account_status::EDITED, "EDITED"},
         {account_status::INACTIVE, "INACTIVE"},
         {account_status::KYC_SUBMITTED, "KYC_SUBMITTED"},
         {account_status::LIMITED, "LIMITED"},
         {account_status::ONBOARDING, "ONBOARDING"},
         {account_status::PAPER_ONLY, "PAPER_ONLY"},
         {account_status::REAPPROVAL_PENDING, "REAPPROVAL_PENDING"},
         {account_status::REJECTED, "REJECTED"},
         {account_status::RESUBMITTED, "RESUBMITTED"},
         {account_status::SIGNED_UP, "SIGNED_UP"},
         {account_status::SUBMISSION_FAILED, "SUBMISSION_FAILED"},
         {account_status::SUBMITTED, "SUBMITTED"}}};
};

template<>
struct enum_mapping<asset_exchange>
{
    static constexpr std::array<std::pair<asset_exchange, std::string_view>, 13> mappings{
        {{asset_exchange::AMEX, "AMEX"},
         {asset_exchange::ARCA, "ARCA"},
         {asset_exchange::BATS, "BATS"},
         {asset_exchange::NYSE, "NYSE"},
         {asset_exchange::NASDAQ, "NASDAQ"},
         {asset_exchange::NYSEARCA, "NYSEARCA"},
         {asset_exchange::OTC, "OTC"},
         {asset_exchange::FTXU, "FTXU"},
         {asset_exchange::CBSE, "CBSE"},
         {asset_exchange::GNSS, "GNSS"},
         {asset_exchange::ERSX, "ERSX"},
         {asset_exchange::CRYPTO, "CRYPTO"},
         {asset_exchange::EMPTY, ""}}};
};

template<>
struct enum_mapping<asset_class>
{
    static constexpr std::array<std::pair<asset_class, std::string_view>, 3> mappings{
        {{asset_class::US_EQUITY, "us_equity"},
         {asset_class::US_OPTION, "us_option"},
         {asset_class::CRYPTO, "crypto"}}};
};

template<>
struct enum_mapping<position_side>
{
    static constexpr std::array<std::pair<position_side, std::string_view>, 2> mappings{
        {{position_side::LONG, "long"}, {position_side::SHORT, "short"}}};
};

template<>
struct enum_mapping<order_side>
{
    static constexpr std::array<std::pair<order_side, std::string_view>, 2> mappings{
        {{order_side::BUY, "buy"}, {order_side::SELL, "sell"}}};
};

template<>
struct enum_mapping<order_class>
{
    static constexpr std::array<std::pair<order_class, std::string_view>, 5> mappings{
        {{order_class::SIMPLE, "simple"},
         {order_class::BRACKET, "bracket"},
         {order_class::OCO, "oco"},
         {order_class::OTO, "oto"},
         {order_class::MLEG, "mleg"}}};
};

template<>
struct enum_mapping<order_type>
{
    static constexpr std::array<std::pair<order_type, std::string_view>, 5> mappings{
        {{order_type::MARKET, "market"},
         {order_type::LIMIT, "limit"},
         {order_type::STOP, "stop"},
         {order_type::STOP_LIMIT, "stop_limit"},
         {order_type::TRAILING_STOP, "trailing_stop"}}};
};

template<>
struct enum_mapping<time_in_force>
{
    static constexpr std::array<std::pair<time_in_force, std::string_view>, 6> mappings{
        {{time_in_force::DAY, "day"},
         {time_in_force::GTC, "gtc"},
         {time_in_force::OPG, "opg"},
         {time_in_force::CLS, "cls"},
         {time_in_force::IOC, "ioc"},
         {time_in_force::FOK, "fok"}}};
};

template<>
struct enum_mapping<order_status>
{
    static constexpr std::array<std::pair<order_status, std::string_view>, 16> mappings{
        {{order_status::NEW, "new"},
         {order_status::PARTIALLY_FILLED, "partially_filled"},
         {order_status::FILLED, "filled"},
         {order_status::DONE_FOR_DAY, "done_for_day"},
         {order_status::CANCELED, "canceled"},
         {order_status::EXPIRED, "expired"},
         {order_status::REPLACED, "replaced"},
         {order_status::PENDING_CANCEL, "pending_cancel"},
         {order_status::PENDING_REPLACE, "pending_replace"},
         {order_status::ACCEPTED, "accepted"},
         {order_status::PENDING_NEW, "pending_new"},
         {order_status::ACCEPTED_FOR_BIDDING, "accepted_for_bidding"},
         {order_status::STOPPED, "stopped"},
         {order_status::REJECTED, "rejected"},
         {order_status::SUSPENDED, "suspended"},
         {order_status::CALCULATED, "calculated"}}};
};

template<>
struct enum_mapping<position_intent>
{
    static constexpr std::array<std::pair<position_intent, std::string_view>, 4> mappings{
        {{position_intent::BUY_TO_OPEN, "buy_to_open"},
         {position_intent::BUY_TO_CLOSE, "buy_to_close"},
         {position_intent::SELL_TO_OPEN, "sell_to_open"},
         {position_intent::SELL_TO_CLOSE, "sell_to_close"}}};
};

template<typename EnumType>
void enum_value_from_tag(json::value& jv, const EnumType& value)
{
    constexpr auto& mappings = enum_mapping<EnumType>::mappings;

    auto it =
        std::find_if(mappings.begin(), mappings.end(), [value](const auto& mapping) { return mapping.first == value; });

    if (it != mappings.end())
    {
        jv = std::string{it->second};
        return;
    }

    throw std::invalid_argument("Invalid enum value for serialization");
}

template<typename EnumType>
EnumType enum_value_to_tag(const json::value& jv)
{
    const std::string value_str = json::value_to<std::string>(jv);
    constexpr auto&   mappings  = enum_mapping<EnumType>::mappings;

    auto it = std::find_if(
        mappings.begin(), mappings.end(), [&value_str](const auto& mapping) { return mapping.second == value_str; });

    if (it != mappings.end())
    {
        return it->first;
    }

    throw std::invalid_argument("Invalid enum string: " + value_str);
}

template<typename EnumType>
EnumType enum_value_to_tag_with_fallback(const json::value& jv, EnumType fallback)
{
    const std::string value_str = json::value_to<std::string>(jv);
    constexpr auto&   mappings  = enum_mapping<EnumType>::mappings;

    auto it = std::find_if(
        mappings.begin(), mappings.end(), [&value_str](const auto& mapping) { return mapping.second == value_str; });

    if (it != mappings.end())
    {
        return it->first;
    }

    if (value_str.empty())
    {
        return fallback;
    }

    throw std::invalid_argument("Invalid enum string: " + value_str);
}
