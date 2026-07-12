#include "MatchingEngine.h"
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

using Symbol = std::string;

void MatchingEngine::processOrder(Order* order) {
    if (order == nullptr) {
        throw std::invalid_argument("Order cannot be null");
    }
    if (order->getSide() == Side::BUY) {
        processBuyOrder(order);
    } else {
        processSellOrder(order);
    }
}

void MatchingEngine::processBuyOrder(Order* order) {
    if (order->getRemainingQuantity() <= 0) {
        throw std::invalid_argument("Quantity must be greater than 0");
    }
    Symbol symbol = order->getSymbol();
    BookSide<std::less<double>>& sellBook = orderBook.getSellBook(symbol);
    match(order, sellBook);
    if (order->getRemainingQuantity() > 0) {
        if (order->getOrderType() == OrderType::LIMIT) {
            orderBook.addOrder(order);
        } else {
            order->cancel();
        }
    }
}

void MatchingEngine::processSellOrder(Order* order) {
    Symbol symbol = order->getSymbol();
    BookSide<std::greater<double>>& buyBook = orderBook.getBuyBook(symbol);
    match(order, buyBook);
    if (order->getRemainingQuantity() > 0) {
        if (order->getOrderType() == OrderType::LIMIT) {
            orderBook.addOrder(order);
        } else {
            order->cancel();
        }
    }
}

template<typename Comparator>
void MatchingEngine::match(Order* incoming, BookSide<Comparator>& oppositeBook) {
    OrderType orderType = incoming->getOrderType();
    Side side = incoming->getSide();

    while (incoming->getRemainingQuantity() > 0 && !oppositeBook.isEmpty()) {
        if (orderType == OrderType::LIMIT) {
            if ((side == Side::BUY && oppositeBook.bestPrice() > incoming->getPrice())
                || (side == Side::SELL && oppositeBook.bestPrice() < incoming->getPrice())) {
                break;
            }
        }
        PriceLevel* level = oppositeBook.bestPriceLevel();
        Order* restingOrder = level->getFrontOrder();
        if (restingOrder == nullptr) {
            break;
        }
        double quantity = std::min(incoming->getRemainingQuantity(), restingOrder->getRemainingQuantity());
        if (quantity > 0) {
            incoming->execute(quantity);
            restingOrder->execute(quantity);
            level->reduceQuantity(quantity);
            if (restingOrder->isFilled()) {
                oppositeBook.removeOrder(restingOrder->getOrderId());
            }
            createTrade(incoming, restingOrder, quantity);
            static int tradeCount = 0;
            ++tradeCount;
            std::cout << "Trade " << tradeCount << std::endl;
            std::cout << static_cast<int>(quantity) << " @" << static_cast<int>(restingOrder->getPrice()) << std::endl;
            std::cout << std::endl;
        }
    }
}

std::unique_ptr<Trade> MatchingEngine::createTrade(Order* incoming, Order* resting, double quantity) {
    int buyOrderId;
    int sellOrderId;
    if (incoming->getSide() == Side::BUY) {
        buyOrderId = incoming->getOrderId();
        sellOrderId = resting->getOrderId();
    } else {
        buyOrderId = resting->getOrderId();
        sellOrderId = incoming->getOrderId();
    }
    return std::make_unique<Trade>(
        buyOrderId,
        sellOrderId,
        incoming->getSymbol(),
        resting->getPrice(),
        quantity,
        incoming->getSide()
    );
}

template<typename Comparator>
void MatchingEngine::removeIfFilled(Order* order, BookSide<Comparator>& side) {
    if (order->isFilled()) {
        side.removeOrder(order->getOrderId());
    }
}

void MatchingEngine::printSellBook(const Symbol& symbol) const {
    orderBook.printSellBook(symbol);
}

void MatchingEngine::printBuyBook(const Symbol& symbol) const {
    orderBook.printBuyBook(symbol);
}

void MatchingEngine::printBooks(const Symbol& symbol) const {
    orderBook.printBooks(symbol);
}

// Explicit template instantiations used by MatchingEngine.cpp
template void MatchingEngine::match<std::less<double>>(Order*, BookSide<std::less<double>>&);
template void MatchingEngine::match<std::greater<double>>(Order*, BookSide<std::greater<double>>&);
