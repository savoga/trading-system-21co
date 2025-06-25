#include <string>
#include <iostream>
#include "Book.hpp"

Book::Book(double amt_start)
        : amount(amt_start), amount_start(amt_start), inventory(0),
          avg_entry_price(0), pnl_total(0), pnl_realized(0), pnl_unrealized(0) {}
void Book::update(double qty, double price, double mid_price) {
    // qty > 0 -> sell
    // qty < 0 -> buy
    double executed_value = qty * price;
    double old_inventory = inventory;
    // same direction -> update average price
    if ((inventory >= 0 && qty >= 0) || (inventory <= 0 && qty <= 0)) {
        double new_inventory = inventory + qty;
        if (new_inventory != 0) {
            avg_entry_price = (avg_entry_price * std::abs(inventory) + price * std::abs(qty)) / std::abs(new_inventory);
        } else {
            avg_entry_price = 0;
        }
        inventory = new_inventory;
    } 
    // opposite direction -> realized P&L
    else {
        if (std::abs(qty) <= std::abs(inventory)) {
            // partial closing
            double realized = std::abs(qty) * (price - avg_entry_price) * (inventory > 0 ? 1 : -1);
            pnl_realized += realized;
            inventory += qty;
        } else {
            double closing_qty = -inventory;
            double opening_qty = qty + inventory;
            double realized = std::abs(closing_qty) * (price - avg_entry_price) * (inventory > 0 ? 1 : -1);
            pnl_realized += realized;
            avg_entry_price = price;
            inventory = opening_qty;
        }
    }
    pnl_unrealized = inventory * (mid_price - avg_entry_price);
    pnl_total = pnl_realized + pnl_unrealized;
    std::cout << "Inventory: " << inventory 
              << ", Avg Entry: " << avg_entry_price 
              << ", Realized PnL: " << pnl_realized
              << ", Unrealized PnL: " << pnl_unrealized
              << ", Total PnL: " << pnl_total << std::endl;
}