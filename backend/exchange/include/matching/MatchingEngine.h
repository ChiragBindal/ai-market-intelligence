#pragma once

#include <memory>
#include "OrderBook.h"
#include "Order.h"
#include "Trade.h"
#include "BookSide.h"

class MatchingEngine {
private:
    OrderBook orderBook;

    template<typename Comparator>
    void match(Order* incoming, BookSide<Comparator>& oppositeBook);

    std::unique_ptr<Trade> createTrade(Order* incoming, Order* resting, double quantity);

    template<typename Comparator>
    void removeIfFilled(Order* order, BookSide<Comparator>& side);

public:
    void processOrder(Order* order);
    void printBuyBook(const Symbol& symbol) const;
    void printSellBook(const Symbol& symbol) const;
    void printBooks(const Symbol& symbol) const;

private:
    void processBuyOrder(Order* order);
    void processSellOrder(Order* order);
};
