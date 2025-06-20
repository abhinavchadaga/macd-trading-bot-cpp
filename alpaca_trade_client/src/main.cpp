#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/alpaca_trade_client.hpp"
#include "alpaca_trade_client/orders.hpp"
#include "alpaca_trade_client/position.hpp"

#include <iostream>

int main()
{
    net::io_context                   ioc;
    const alpaca_trade_client::config cfg{"PKPPQ1GGQQ3FVEACBGQG", "9KRgrTQdZ4dULrlViIgukby7ZRyQPbcynV3aNNRN"};
    auto                              trade_client{alpaca_trade_client::create(ioc, cfg)};

    auto fut{net::co_spawn(
        ioc.get_executor(),
        [trade_client]() -> net::awaitable<void>
        {
            // Step 1: Check buying power
            std::string buying_power{};
            if (auto result = co_await trade_client->account())
            {
                if (const auto& account{result.value()}; account.buying_power.has_value())
                {
                    buying_power = account.buying_power.value();
                    std::cout << "Available buying power: $" << buying_power << std::endl;
                }
                else
                {
                    std::cout << "Buying power not available" << std::endl;
                    co_return;
                }
            }
            else
            {
                const auto& error = result.error();
                std::cerr << "Error getting account: " << error.message() << " (HTTP status: " << error.http_status()
                          << ")" << std::endl;
                co_return;
            }

            // Step 2: Place notional order for PLTR with full buying power
            notional_order pltr_order{
                .symbol = "PLTR", .notional = buying_power, .side = order_side::BUY, .extended_hours = false};

            std::cout << "Placing order for PLTR with $" << buying_power << " notional..." << std::endl;

            if (auto order_result = co_await trade_client->create_order(pltr_order))
            {
                const auto&    order       = order_result.value();
                nlohmann::json side_json   = order.side;
                nlohmann::json status_json = order.status;
                std::cout << "Order placed successfully: " << order.symbol << " " << side_json.get<std::string>()
                          << " $" << order.notional.value_or("N/A") << " (Status: " << status_json.get<std::string>()
                          << ", ID: " << order.id << ")" << std::endl;
            }
            else
            {
                const auto& error = order_result.error();
                std::cerr << "Error placing order: " << error.message() << " (HTTP status: " << error.http_status()
                          << ")" << std::endl;
                co_return;
            }

            // Step 3: Cancel all orders (should see the order we just placed)
            std::cout << "Cancelling all open orders..." << std::endl;

            if (auto delete_result = co_await trade_client->delete_all_orders())
            {
                const auto& deleted_orders = delete_result.value();
                std::cout << "Successfully cancelled " << deleted_orders.size() << " order(s):" << std::endl;
                for (const auto& deleted_order : deleted_orders)
                {
                    std::cout << "  Order ID: " << deleted_order.id << " (HTTP status: " << deleted_order.status << ")"
                              << std::endl;
                }
            }
            else
            {
                const auto& error = delete_result.error();
                std::cerr << "Error cancelling orders: " << error.message() << " (HTTP status: " << error.http_status()
                          << ")" << std::endl;
            }
        },
        net::use_future)};

    ioc.run();
    fut.get();
    return 0;
}
