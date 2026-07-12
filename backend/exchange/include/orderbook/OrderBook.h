#pragma once

#include <map>
#include <string>
#include "BookSide.h"
#include "Order.h"
#include "PriceLevel.h"
#include "Side.h"

using Symbol = std::string;

class OrderBook {
private:
    std::map<Symbol, BookSide<std::greater<double>>> buyBook;
    std::map<Symbol, BookSide<std::less<double>>> sellBook;

public:
    OrderBook() = default;
    void addOrder(Order* order);
    void removeOrder(Order* order);
    bool isEmpty(Side side, Symbol symbol);
    PriceLevel* bestPriceLevel(Side side, Symbol symbol);
    bool hasSymbol(Symbol symbol) const;
    BookSide<std::greater<double>>& getBuyBook(Symbol symbol);
    BookSide<std::less<double>>& getSellBook(Symbol symbol);
    void printBuyBook(Symbol symbol) const;
    void printSellBook(Symbol symbol) const;
    void printBooks(Symbol symbol) const;
};
