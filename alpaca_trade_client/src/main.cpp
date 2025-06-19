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
            if (auto result = co_await trade_client->effective_buying_power())
            {
                std::cout << "Effective buying power: $" << result.value() << std::endl;
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
