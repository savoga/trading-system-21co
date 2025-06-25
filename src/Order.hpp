#pragma once

#include <string>
#include "Book.hpp"

class Order {
public:
    int side;
    double price;
    double quantity;
    double price_executed;
    double quantity_executed;
    std::string comment;
    double cost;
    double max_order_size = 50000;
    double max_total_position = 5;
    int max_order_speed = 5;
    double max_daily_loss = 10000;
    void print_order() const;
    bool check(Book& b, double mid_price);
};