#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <deque>
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iomanip>
#include <random>

#include "Book.hpp"
#include "BinanceWebSocket.hpp"
#include "OrderBook.hpp"
#include "OrderSimulation.hpp"

int main() {
    try {
        Book b(200000);

        BinanceWebSocket ws;
        ws.start();

        OrderBook ob;
        std::thread thread_update([&]() { ob.update_order_book(ws); });
        std::thread thread_executor([&]() { ob.execute_orders(b); });

        OrderSimulation os;
        std::thread thread_listener([&]() { os.listen(ob); });
        std::thread thread_simulator([&]() { os.simulate_orders(ob); });

        ws.join();
        thread_update.join();
        thread_executor.join();
        thread_listener.join();
        thread_simulator.join();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}