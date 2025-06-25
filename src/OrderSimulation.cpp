#include <random>
#include <thread>

#include "OrderBook.hpp"
#include "OrderSimulation.hpp"

void OrderSimulation::listen(OrderBook& orderBook) {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream iss(line);
        std::string side_str;
        double price, quantity;
        if (!(iss >> side_str >> price >> quantity)) {
            std::cerr << "Invalid order format. Use: BUY/SELL <price> <quantity>\n";
            continue;
        }
        int side;
        if (side_str == "BUY") {
            side = 1;
        } else if (side_str == "SELL") {
            side = -1;
        } else {
            std::cerr << "Invalid side. Use BUY or SELL.\n";
            continue;
        }
        Order order{side, price, quantity};
        {
            std::lock_guard<std::mutex> lock(order_mtx);
            orders.push_back(order);
        }
        order.print_order();
        orderBook.submit_order(order);
        }  
}
void OrderSimulation::simulate_orders(OrderBook& orderBook) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis_side(0, 1);
    std::uniform_real_distribution<> dis_quantity(quantity_low, quantity_high);
    std::uniform_real_distribution<> dis_price(price_low, price_high);
    std::uniform_int_distribution<> dis_milliseconds(milliseconds_low, milliseconds_high);
    while (true) {
        int side = dis_side(gen) == 0 ? -1 : 1;
        double quantity = dis_quantity(gen);
        double price = dis_price(gen);
        int milliseconds = dis_milliseconds(gen);
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        Order order{side, price, quantity};
        order.print_order();
        {
            std::lock_guard<std::mutex> lock(order_mtx);
            orders.push_back(order);
        }
        orderBook.submit_order(order);
    }
}