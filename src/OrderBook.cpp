#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include "OrderBook.hpp"
#include "Book.hpp"
#include "Order.hpp"
#include "CommonTypes.hpp"
#include "BinanceWebSocket.hpp"
#include "NetworkUtils.hpp"

void OrderBook::update_order_book(BinanceWebSocket& ws) {
    std::thread print_thread([this]() { print_top_of_book(); });
    std::unique_lock<std::mutex> lock(ws.mtx);
    ws.cv.wait(lock, [&ws]() { return !ws.message_buffer.empty(); }); // wait to have at least some updates
    // get first snapshot
    json snapshot = fetch_snapshot();
    lastUpdateId = snapshot["lastUpdateId"];
    for (auto& b : snapshot["bids"]) bids[std::stod(b[0].get<std::string>())] = std::stod(b[1].get<std::string>());
    for (auto& a : snapshot["asks"]) asks[std::stod(a[0].get<std::string>())] = std::stod(a[1].get<std::string>());
    while (!ws.message_buffer.empty()) {
        std::string msg_raw = ws.message_buffer.front(); // oldest received event
        json msg = json::parse(msg_raw);
        int64_t U = msg["U"], u = msg["u"];
        while (lastUpdateId < U) {
            // Test A/1. snapshot is too old!!
            std::cout << "Snapshot is too old. Retrying to get a snapshot to start with." << std::endl;
            json snapshot = fetch_snapshot();
            lastUpdateId = snapshot["lastUpdateId"];
            for (auto& b : snapshot["bids"]) bids[std::stod(b[0].get<std::string>())] = std::stod(b[1].get<std::string>());
            for (auto& a : snapshot["asks"]) asks[std::stod(a[0].get<std::string>())] = std::stod(a[1].get<std::string>());
        }
        std::cout << "Good snapshot found." << std::endl;
        if (u < lastUpdateId) {
            // Test A/2. event is ignored because all updates are already in the snapshot
            // => we remove the event
            ws.message_buffer.pop_front();
            continue;
        }
        ws.message_buffer.pop_front();
    }
    lock.unlock(); // to get messages again
    reconcile(ws);
    print_thread.join();
}
void OrderBook::execute_orders(Book& book) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // periodic check
        std::lock_guard<std::mutex> lock(order_exec_mtx);
        std::lock_guard<std::mutex> book_lock(book_mtx);
        std::deque<Order> still_pending;
        for (auto& order : pending_orders) {
            bool is_rejected = false;
            double best_bid = bids.begin()->first;
            double best_ask = asks.begin()->first;
            double mid_price = (best_bid+best_ask)/2;
            double qty_remaining = order.quantity;
            if (order.side == 1) { // BUY order
                auto it = asks.begin(); // we go through all asks
                while (it != asks.end() && it->first <= order.price && qty_remaining > 0) {
                    double match_qty = std::min(qty_remaining, it->second);
                    if (order.check(book, mid_price) == true) {
                        std::cout << "Executed BUY at " << it->first << " for " << match_qty << "\n";
                        book.update(match_qty, it->first, mid_price);
                        qty_remaining -= match_qty;
                        it->second -= match_qty;
                        if (it->second <= 0) it = asks.erase(it);
                        else ++it;
                    } else {
                        is_rejected = true;
                        rejected_orders.push_back(order);
                        break;
                    }
                }
            } else { // SELL order
                auto it = bids.begin();
                while (it != bids.end() && it->first >= order.price && qty_remaining > 0) {
                    double match_qty = std::min(qty_remaining, it->second);
                    if (order.check(book, mid_price) == true) {
                        std::cout << "Executed SELL at " << it->first << " for " << match_qty << "\n";
                        book.update(-1*match_qty, it->first, mid_price);
                        qty_remaining -= match_qty;
                        it->second -= match_qty;
                        if (it->second <= 0) it = bids.erase(it);
                        else ++it;
                    } else {
                        is_rejected = true;
                        rejected_orders.push_back(order);
                        break;
                    }
                }
            }
            if (qty_remaining > 0 && is_rejected!=true) {
                std::cout << "Unfilled order remains: " << (order.side == 1 ? "BUY" : "SELL")
                        << " " << order.price << " " << qty_remaining << "\n";
                still_pending.push_back(Order{order.side, order.price, qty_remaining});
            }
        }
        pending_orders = std::move(still_pending);
    }
}
void OrderBook::submit_order(const Order& order) {
    std::lock_guard<std::mutex> lock(order_exec_mtx);
    pending_orders.push_back(order);
}

void OrderBook::reconcile(BinanceWebSocket& ws) {
    std::unique_lock<std::mutex> lock(ws.mtx);
    while (true) {
        while (!ws.message_buffer.empty()) {
            std::string raw = ws.message_buffer.front();
            ws.message_buffer.pop_front();
            lock.unlock();
            json msg = json::parse(raw);
            int64_t u = msg["u"];
            if (u <= lastUpdateId) {
                // event is ignored because all updates are already in the snapshot
                lock.lock();
                continue;
            }
            update(bids, msg["b"], true);
            update(asks, msg["a"], false);
            lastUpdateId = u;
            lock.lock();
        }
        ws.cv.wait(lock);
    }
}
void OrderBook::print_top_of_book() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::lock_guard<std::mutex> lock(book_mtx);
        if (!bids.empty() && !asks.empty()) {
            double best_bid = bids.begin()->first;
            double best_bid_qty = bids.begin()->second;
            double best_ask = asks.begin()->first;
            double best_ask_qty = asks.begin()->second;
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            if (std::cin.rdbuf()->in_avail() == 0) {  // only print if user isn't typing
                std::cout << std::put_time(std::localtime(&now), "%H:%M:%S")
                    << " Best Bid: " << std::fixed << std::setprecision(4) << best_bid
                    << " (" << std::setprecision(2) << best_bid_qty << ")"
                    << " | Best Ask: " << std::fixed << std::setprecision(4) << best_ask
                    << " (" << std::setprecision(2) << best_ask_qty << ")"
                    << std::endl;
            }
        }
    }
}