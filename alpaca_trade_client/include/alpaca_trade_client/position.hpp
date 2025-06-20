#pragma once

#include <nlohmann/json.hpp>
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
    asset_class                asset_class{};
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

inline void to_json(nlohmann::json& j, const asset_exchange& exchange)
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

inline void from_json(const nlohmann::json& j, asset_exchange& exchange)
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

inline void to_json(nlohmann::json& j, const asset_class& asset_class_type)
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

inline void from_json(const nlohmann::json& j, asset_class& asset_class_type)
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

inline void to_json(nlohmann::json& j, const position_side& side)
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

inline void from_json(const nlohmann::json& j, position_side& side)
{
    if (const std::string side_str = j; side_str == "long")
        side = position_side::LONG;
    else if (side_str == "short")
        side = position_side::SHORT;
    else
        throw std::invalid_argument("Invalid position_side: " + side_str);
}

inline void from_json(const nlohmann::json& j, position& pos)
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

inline void to_json(nlohmann::json& j, const position& pos)
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
