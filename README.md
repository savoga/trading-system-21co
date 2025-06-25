# Trading system - 21.co hiring process

# Intro

I've done this project as part of an interview process for the company 21.co. 

The code implements a multithreaded C++ trading simulator that connects to Binance via websocket to stream real time market depth data and maintain an up-to-date local order book. 

It first fetches a full depth snapshot over HTTPS, then applies streamed updates to stay synchronized. Users can place simulated buy/sell orders via terminal input or automatic random generation, which are matched against the local order book. A Book class tracks inventory, average entry price, and both realized and unrealized P&L as orders are executed. The system prints top-of-book data periodically and includes basic risk checks like max order size and position limits.

# Setup

- C++ version: 17

- Compiler: Clang

- Important libraries: Boost/Websocket

# How to run it

I usually run it either from my Visual Studio Code console or in the terminal with the command:

```clang++ -Iinclude/websocketpp -Iinclude/vcpkg/installed/arm64-osx/include -Linclude/vcpkg/installed/arm64-osx/lib -lssl -lcrypto -std=c++17 src/main.cpp -o main``` then ```./main```

To insert orders, the user needs to use the terminal.

# Improvements

Many improvements could be added, some examples:

- refactoring:
  - OrderBook could be split in different classes
  - docstring generation
  - better error handling

- unit testing
  - logic risk
  - order routing logic

- logic sanity checks:
  - I somehow feel that I am missing some events because not all tests recommended on Binance are successful => some tests should be added to make sure of the events' chronology
 
- speed:
  - consider using @depth instead of @depth100ms
  - lock/unlock can probably be further improved
 
- additional functionalities:
  - save all trades in a database
  - simulate slippage
  - consider fees
