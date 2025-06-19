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
            if (auto result = co_await trade_client->account())
            {
                const std::string effective_buying_power{result.value()["effective_buying_power"]};
                std::cout << "Account info: $" << effective_buying_power << " " << std::endl;
            }
            else
            {
                const auto& error = result.error();
                std::cerr << "Error: " << error.message() << " (HTTP status: " << error.http_status() << ")"
                          << std::endl;
            }

            if (auto result = co_await trade_client->all_open_positions())
            {
                const std::size_t num_open_pos{result.value().size()};
                std::cout << "number of open positions: " << num_open_pos << std::endl;
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
