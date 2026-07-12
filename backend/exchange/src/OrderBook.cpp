#include "OrderBook.h"

#include <iostream>

void OrderBook::addOrder(Order* order) {
    if (!hasSymbol(order->getSymbol())) {
        buyBook[order->getSymbol()] = BookSide<std::greater<double>>();
        sellBook[order->getSymbol()] = BookSide<std::less<double>>();
    }
    if (order->getSide() == Side::BUY) {
        buyBook[order->getSymbol()].addOrder(order);
    } else {
        sellBook[order->getSymbol()].addOrder(order);
    }
}

void OrderBook::removeOrder(Order* order) {
    if (order->getSide() == Side::BUY) {
        buyBook[order->getSymbol()].removeOrder(order->getOrderId());
    } else {
        sellBook[order->getSymbol()].removeOrder(order->getOrderId());
    }
}

bool OrderBook::isEmpty(Side side, Symbol symbol) {
    if (side == Side::BUY) {
        return buyBook[symbol].isEmpty();
    }
    return sellBook[symbol].isEmpty();
}

PriceLevel* OrderBook::bestPriceLevel(Side side, Symbol symbol) {
    if (side == Side::BUY) {
        return buyBook[symbol].bestPriceLevel();
    }
    return sellBook[symbol].bestPriceLevel();
}

bool OrderBook::hasSymbol(Symbol symbol) const {
    return buyBook.find(symbol) != buyBook.end() && sellBook.find(symbol) != sellBook.end();
}

BookSide<std::greater<double>>& OrderBook::getBuyBook(Symbol symbol) {
    return buyBook[symbol];
}

BookSide<std::less<double>>& OrderBook::getSellBook(Symbol symbol) {
    return sellBook[symbol];
}

void OrderBook::printBuyBook(Symbol symbol) const {
    auto it = buyBook.find(symbol);
    if (it == buyBook.end() || it->second.isEmpty()) {
        return;
    }
    std::cout << "BUY BOOK" << std::endl;
    it->second.forEachLevel([](Price price, int quantity) {
        std::cout << static_cast<int>(price) << " : " << quantity << std::endl;
    });
}

void OrderBook::printSellBook(Symbol symbol) const {
    auto it = sellBook.find(symbol);
    if (it == sellBook.end() || it->second.isEmpty()) {
        return;
    }
    std::cout << "SELL BOOK" << std::endl;
    it->second.forEachLevel([](Price price, int quantity) {
        std::cout << static_cast<int>(price) << " : " << quantity << std::endl;
    });
}

void OrderBook::printBooks(Symbol symbol) const {
    printBuyBook(symbol);
    if (!buyBook.empty() && !sellBook.empty()) {
        auto buyIt = buyBook.find(symbol);
        auto sellIt = sellBook.find(symbol);
        if (buyIt != buyBook.end() && !buyIt->second.isEmpty()
            && sellIt != sellBook.end() && !sellIt->second.isEmpty()) {
            std::cout << std::endl;
        }
    }
    printSellBook(symbol);
}
