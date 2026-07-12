#include "MatchingEngine.h"
#include "Order.h"

#include <iostream>
#include <memory>

int main() {
    MatchingEngine engine;
    const std::string symbol = "AAPL";

    // SELL BOOK: 101 : 100
    auto sell = std::make_unique<Order>(symbol, Side::SELL, OrderType::LIMIT, 101.0, 100.0);

    // Incoming: BUY LIMIT 100 @ 100
    auto buy = std::make_unique<Order>(symbol, Side::BUY, OrderType::LIMIT, 100.0, 100.0);

    engine.processOrder(sell.get());
    engine.processOrder(buy.get());

    engine.printBooks(symbol);

    return 0;
}
