#include <string>
#include <iostream>
#include "Order.hpp"

void Order::print_order() const {
    std::cout << "Received order: " << (side == 1 ? "BUY" : "SELL")
              << " " << price << " " << quantity << std::endl;
}
bool Order::check(Book& b, double mid_price) {
    std::string rejection_comment = "";
    if ((price*quantity)>max_order_size) {
        rejection_comment = "Order rejected. Notional exceeds " + std::to_string(max_order_size) + " limit.";
        std::cout << rejection_comment << std::endl;
        comment = rejection_comment;
        return false;
    }
    if (std::abs(b.inventory) > max_total_position) {
        rejection_comment = "Order rejected. Total position exceeds " + std::to_string(max_total_position) + " BTC.";
        std::cout << rejection_comment << std::endl;
        comment = rejection_comment;
        return false;
    }
    return true;
}