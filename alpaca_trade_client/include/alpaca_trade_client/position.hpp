#pragma once

#include <boost/json.hpp>
#include <optional>
#include <string>

enum class asset_exchange
{
    AMEX,
    ARCA,
    BATS,
    NYSE,
    NASDAQ,
    NYSEARCA,
    FTXU,
    CBSE,
    GNSS,
    ERSX,
    OTC,
    CRYPTO,
    EMPTY
};

enum class asset_class
{
    US_EQUITY,
    US_OPTION,
    CRYPTO
};

enum class position_side
{
    LONG,
    SHORT
};

struct position
{
    std::string                asset_id{};
    std::string                symbol{};
    asset_exchange             exchange{};
    asset_class                asset_class_type{};
    std::string                avg_entry_price{};
    std::string                qty{};
    std::optional<std::string> qty_available{};
    position_side              side{};
    std::string                market_value{};
    std::string                cost_basis{};
    std::string                unrealized_pl{};
    std::string                unrealized_plpc{};
    std::string                unrealized_intraday_pl{};
    std::string                unrealized_intraday_plpc{};
    std::string                current_price{};
    std::string                lastday_price{};
    std::string                change_today{};
    bool                       asset_marginable{};
};

void           tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const asset_exchange& exchange);
asset_exchange tag_invoke(boost::json::value_to_tag<asset_exchange>, const boost::json::value& jv);

void        tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const asset_class& asset_class_type);
asset_class tag_invoke(boost::json::value_to_tag<asset_class>, const boost::json::value& jv);

void          tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const position_side& side);
position_side tag_invoke(boost::json::value_to_tag<position_side>, const boost::json::value& jv);

void     tag_invoke(boost::json::value_from_tag, boost::json::value& jv, const position& pos);
position tag_invoke(boost::json::value_to_tag<position>, const boost::json::value& jv);
