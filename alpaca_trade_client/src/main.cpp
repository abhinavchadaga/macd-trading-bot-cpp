#include "alpaca_trade_client/account.hpp"
#include "alpaca_trade_client/alpaca_trade_client.hpp"

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
            std::string buying_power{};
            if (auto result = co_await trade_client->account())
            {
                if (const auto& account{result.value()}; account.buying_power.has_value())
                {
                    buying_power = account.buying_power.value();
                    std::cout << "Buying Power: $" << buying_power << std::endl;
                }
                else
                {
                    std::cout << "Buying power not available" << std::endl;
                }
            }
            else
            {
                const auto& error = result.error();
                std::cerr << "Error: " << error.message() << " (HTTP status: " << error.http_status() << ")"
                          << std::endl;
            }
        },
        net::use_future)};

    ioc.run();
    fut.get();
    return 0;
}
