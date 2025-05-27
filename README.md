Simple trading bot that uses MACD crossovers to buy and sell a single symbol. The bot is given a certain budget and a symbol to trade. For example, we give the bot $100 and the symbol PLTR. After some time, we hope to come back and see $250 in the account for the bot after the bot as purchased and sold PLTR several times to make a profit.

## Strategy

trade upward trends on a well traded symbol (SPY, TSLA, PLTR, etc.)

### Entry Condition

1. `close price > EMA_200`
2. MACD line cross **above** the MACD signal line
3. MACD line is **below** zero
4. ATR / close price < ATR threshold

All four conditions must be met to trigger a **Market** buy

After placing a **Market** buy, we put a stop loss on the order with a limit of `stop_price = entry_price - (1.5 * atr)`

### Exit Conditions

1. `close price < EMA_200`
2. MACD line crosses below the MACD signal line

# Tech Stack

1. modern C++20 written to follow Object Oriented Design
2. Alpaca Trading + Market Data API
3. Websockets
4. Boost.Beast for networking calls

# Architecture

The underlying design for this bot will be to follow the **Reactor** pattern - modeled after the general program flow that Virtu Financial follows with their code. The bot will operate using a single-threaded, event loop driven architecture to pull in market data, react accordingly, and track the current position that the bot is taking. Alpaca uses websockets to stream data. We will be interfacing with the websockets in two ways:

1. To get real-time market data
2. To track orders that we place and react to fills

## Program Flow

The bot is always in one of two states. It has either created a long position on the symbol by making a **Market** purchase equal to the notional amount of the account waiting for a **sell** signal. Or it is holding 100% cash position waiting for a **buy** signal to enter a long position. Below is the program flow of the bot, centered around the initial input of receiving a one-minute candle from Alpaca’s WebSocket market data endpoint.

1. Receive One Minute Candle
2. Check current position of bot through Alpaca
    1. In case we had a stop limit order fill for example - the bot is no longer in a **long** state
3. Consolidate one minute candles into 5 minute candles
4. On computing a 5 minute candle, compute indicators
5. Strategy logic looks at indicators. Outputs either **BUY**, **SELL**, or **NO ACTION**
6. TradeEngine looks at Strategy decision and places the appropriate market order
    1. If buy, place **Market** buy for notional amount equal to account cash balance. On fill, place a stop loss order with limit equal to `entry_price - (1.5 * atr)`
    2. If sell, place **Market** sell order for whole position
    3. If no action, do nothing

This program flow continues for all normal market hours. 15 minutes before the market closes, any open positions will be closed to ensure that we are flat during after market hours.

We want a modular design to ensure that components can we mocked and back tested. For example, the market stream should either be **Live** or **Historical/Mock.** The trading client should either be **live** or **mocked** (not alpaca paper!). With these mockable components, we would then be able to run the bot on historical data and see the how profitable it is on various symbols.
