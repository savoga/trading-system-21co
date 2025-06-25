#pragma once

class Book {
public:
    double amount;
    double amount_start;
    double inventory;
    double avg_entry_price;
    double pnl_total;
    double pnl_realized;
    double pnl_unrealized;
    Book(double amt_start);
    void update(double qty, double price, double mid_price);
};
