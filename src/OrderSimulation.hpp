#pragma once

#include <deque>
#include <mutex>
#include "Order.hpp"
#include "OrderBook.hpp"

class OrderSimulation {
private:
    std::deque<Order> orders;
    std::mutex order_mtx;
    double quantity_low = 0;
    double quantity_high = 0.4;
    double price_low = 107900;
    double price_high = 108900;
    int milliseconds_low = 10;
    int milliseconds_high = 100;
public:
    void listen(OrderBook& orderBook);
    void simulate_orders(OrderBook& orderBook);
};