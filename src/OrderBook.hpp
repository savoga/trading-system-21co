#pragma once

#include <iostream>
#include <deque>
#include <map>
#include <mutex>
#include <nlohmann/json.hpp>

#include "Order.hpp"
#include "Book.hpp"
#include "BinanceWebSocket.hpp"

class OrderBook {
private:
    std::map<double, double, std::greater<>> bids;
    std::map<double, double> asks;
    int64_t lastUpdateId = 0;
    std::mutex book_mtx;
    std::deque<Order> pending_orders;
    std::deque<Order> rejected_orders;
    std::mutex order_exec_mtx;
    template<typename Comparator>
    void update(std::map<double, double, Comparator>& book, const json& updates, bool is_bid) {
        std::lock_guard<std::mutex> lock(book_mtx);
        for (auto& level : updates) {
            double price = std::stod(level[0].get<std::string>());
            double qty = std::stod(level[1].get<std::string>());
            if (qty == 0) book.erase(price);
            else book[price] = qty;
        }
    }
    void reconcile(BinanceWebSocket& ws);
    void print_top_of_book();
public:
    void update_order_book(BinanceWebSocket& ws);
    void execute_orders(Book& book);
    void submit_order(const Order& order);

};