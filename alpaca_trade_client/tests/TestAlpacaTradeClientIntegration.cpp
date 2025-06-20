#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/alpaca_trade_client.hpp"
#include "alpaca_trade_client/orders.hpp"
#include "alpaca_trade_client/position.hpp"
#include "my_logger.hpp"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_future.hpp>
#include <chrono>
#include <gtest/gtest.h>
#include <string>

namespace net = boost::asio;

class AlpacaTradeClientIntegrationTest : public testing::Test
{
public:
    static void SetupTestSuite() { init_logger("alpaca_trade_client_tests"); }

protected:
    void SetUp() override
    {
        const char* api_key    = std::getenv("ALPACA_API_KEY");
        const char* api_secret = std::getenv("ALPACA_API_SECRET");

        ASSERT_NE(api_key, nullptr) << "ALPACA_API_KEY environment variable must be set";
        ASSERT_NE(api_secret, nullptr) << "ALPACA_API_SECRET environment variable must be set";

        const alpaca_trade_client::config cfg{api_key, api_secret};
        _client = alpaca_trade_client::create(_ioc, cfg);
    }

    void TearDown() override { _client.reset(); }

    net::io_context                      _ioc{};
    std::shared_ptr<alpaca_trade_client> _client;
};

TEST_F(AlpacaTradeClientIntegrationTest, FullTradingWorkflow)
{
    auto future = net::co_spawn(
        _ioc,
        [this]() -> net::awaitable<void>
        {
            //
            // Step 1: Check account details - should have buying power
            auto account_result = co_await _client->account();
            EXPECT_TRUE(account_result.has_value()) << "Failed to get account: " << account_result.error().message();

            const auto& account = account_result.value();
            EXPECT_FALSE(account.id.empty()) << "Account ID should not be empty";
            EXPECT_TRUE(account.buying_power.has_value()) << "Buying power should be available";
            const auto& buying_power_str = account.buying_power.value();
            EXPECT_FALSE(buying_power_str.empty()) << "Buying power should not be empty";

            double buying_power_value = 0.0;
            EXPECT_NO_THROW(buying_power_value = std::stod(buying_power_str)) << "Buying power should be numeric";
            EXPECT_GT(buying_power_value, 0.0) << "Should have positive buying power";

            std::string order_notional = std::to_string(static_cast<int>(buying_power_value));

            //
            // Step 2: Place market order for notional value
            notional_order test_order{
                .symbol = "PLTR", .notional = order_notional, .side = order_side::BUY, .extended_hours = false};

            auto order_result = co_await _client->create_order(test_order);
            EXPECT_TRUE(order_result.has_value()) << "Failed to create order: " << order_result.error().message();

            if (!order_result.has_value())
                co_return;

            const auto& created_order = order_result.value();
            EXPECT_FALSE(created_order.id.empty()) << "Order ID should not be empty";
            EXPECT_EQ(created_order.symbol, "PLTR") << "Order symbol should match";

            co_await net::steady_timer(_ioc, std::chrono::milliseconds(500)).async_wait(net::use_awaitable);

            //
            // Step 3: Check if order shows up in orders or positions
            bool found_in_orders    = false;
            bool found_in_positions = false;

            auto orders_result = co_await _client->get_all_orders();
            EXPECT_TRUE(orders_result.has_value()) << "Failed to get orders: " << orders_result.error().message();

            if (orders_result.has_value())
            {
                for (const auto& orders = orders_result.value(); const auto& order : orders)
                {
                    if (order.id == created_order.id)
                    {
                        found_in_orders = true;
                        EXPECT_EQ(order.symbol, "PLTR") << "Order symbol should match";
                        break;
                    }
                }
            }

            auto positions_result = co_await _client->all_open_positions();
            EXPECT_TRUE(positions_result.has_value())
                << "Failed to get positions: " << positions_result.error().message();

            if (positions_result.has_value())
            {
                for (const auto& positions = positions_result.value(); const auto& position : positions)
                {
                    if (position.symbol == "PLTR")
                    {
                        found_in_positions = true;
                        EXPECT_FALSE(position.asset_id.empty()) << "Position asset_id should not be empty";
                        break;
                    }
                }
            }

            // Order should be either pending (in orders) or filled (in positions)
            EXPECT_TRUE(found_in_orders || found_in_positions)
                << "Order should appear either in open orders or as a position";

            //
            // Step 4: Close all orders and positions

            auto cancel_result = co_await _client->delete_all_orders();
            EXPECT_TRUE(cancel_result.has_value()) << "Failed to cancel orders: " << cancel_result.error().message();

            if (cancel_result.has_value())
            {
                const auto& cancelled_orders = cancel_result.value();

                if (found_in_orders)
                {
                    bool found_cancelled = false;
                    for (const auto& [id, status] : cancelled_orders)
                    {
                        if (id == created_order.id)
                        {
                            found_cancelled = true;
                            EXPECT_GE(status, 200) << "Cancellation should return HTTP 2xx status";
                            EXPECT_LT(status, 300) << "Cancellation should return HTTP 2xx status";
                            break;
                        }
                    }
                    EXPECT_TRUE(found_cancelled) << "Our order should be in the cancelled list";
                }
            }

            auto close_result = co_await _client->close_all_positions(true);
            EXPECT_TRUE(close_result.has_value()) << "Failed to close positions: " << close_result.error().message();

            if (close_result.has_value())
            {
                const auto& closed_positions = close_result.value();

                if (found_in_positions)
                {
                    bool found_closed = false;
                    for (const auto& closed : closed_positions)
                    {
                        if (closed.symbol == "PLTR")
                        {
                            found_closed = true;
                            EXPECT_NE(closed.status, 0) << "Closure status should not be zero";
                            break;
                        }
                    }
                    EXPECT_TRUE(found_closed) << "Our position should be in the closed list";
                }
            }

            //
            // Final verification: should have no positions or orders for PLTR
            co_await net::steady_timer(_ioc, std::chrono::milliseconds(2000)).async_wait(net::use_awaitable);

            if (auto final_orders_result = co_await _client->get_all_orders(); final_orders_result.has_value())
            {
                for (const auto& final_orders = final_orders_result.value(); const auto& order : final_orders)
                {
                    EXPECT_NE(order.symbol, "PLTR") << "Should have no open PLTR orders after cleanup";
                }
            }

            if (auto final_positions_result = co_await _client->all_open_positions();
                final_positions_result.has_value())
            {
                for (const auto& final_positions = final_positions_result.value();
                     const auto& position : final_positions)
                {
                    EXPECT_NE(position.symbol, "PLTR") << "Should have no open PLTR positions after cleanup";
                }
            }
        },
        net::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}

// Test error handling with invalid credentials
TEST_F(AlpacaTradeClientIntegrationTest, InvalidCredentials)
{
    const alpaca_trade_client::config bad_cfg{"invalid_key", "invalid_secret"};
    auto                              bad_client = alpaca_trade_client::create(_ioc, bad_cfg);

    auto future = net::co_spawn(
        _ioc,
        [&bad_client]() -> net::awaitable<void>
        {
            auto account_result = co_await bad_client->account();
            EXPECT_FALSE(account_result.has_value()) << "Should fail with invalid credentials";

            if (!account_result.has_value())
            {
                const auto& error = account_result.error();
                EXPECT_EQ(error.type(), alpaca_api_error::error_type::http_error) << "Should be HTTP error";
                EXPECT_EQ(error.http_status(), 403) << "Should be 403 Forbidden";
            }
        },
        net::use_future);

    _ioc.run();
    EXPECT_NO_THROW(future.get());
}
