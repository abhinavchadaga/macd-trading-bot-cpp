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

void to_json(nlohmann::json& j, const asset_exchange& exchange);
void from_json(const nlohmann::json& j, asset_exchange& exchange);

void to_json(nlohmann::json& j, const asset_class& asset_class_type);
void from_json(const nlohmann::json& j, asset_class& asset_class_type);

void to_json(nlohmann::json& j, const position_side& side);
void from_json(const nlohmann::json& j, position_side& side);

void from_json(const nlohmann::json& j, position& pos);
void to_json(nlohmann::json& j, const position& pos);
