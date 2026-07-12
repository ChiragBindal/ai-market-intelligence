#pragma once

#include <map>
#include <unordered_map>
#include <stdexcept>
#include "PriceLevel.h"
#include "Order.h"

using Price = double;
using OrderId = int;

template<typename Comparator>
class BookSide {
private:
    std::map<Price, PriceLevel, Comparator> priceLevels;
    std::unordered_map<OrderId, Price> orderIdToPrice;

public:
    BookSide() = default;

    void addOrder(Order* order) {
        if (order == nullptr) {
            throw std::invalid_argument("Order cannot be nullptr");
        }
        if (order->getRemainingQuantity() <= 0) {
            throw std::invalid_argument("Order remaining quantity must be greater than 0");
        }
        Price price = order->getPrice();
        if (priceLevels.find(price) == priceLevels.end()) {
            priceLevels[price] = PriceLevel();
        }
        orderIdToPrice[order->getOrderId()] = price;
        priceLevels[price].addOrder(order);
    }

    void removeOrder(int orderId) {
        if (priceLevels.empty()) {
            throw std::invalid_argument("No price levels in the book side");
        }
        auto it = orderIdToPrice.find(orderId);
        if (it == orderIdToPrice.end()) {
            throw std::invalid_argument("Order not found");
        }
        Price price = it->second;
        priceLevels[price].removeOrder(orderId);
        orderIdToPrice.erase(orderId);
        if (priceLevels[price].getOrderCount() == 0) {
            priceLevels.erase(price);
        }
    }

    bool isEmpty() const {
        return priceLevels.empty();
    }

    double getTotalQuantity(Price price) const {
        auto it = priceLevels.find(price);
        if (it == priceLevels.end()) {
            return 0;
        }
        return it->second.getTotalQuantity();
    }

    template<typename Fn>
    void forEachLevel(Fn fn) const {
        for (const auto& [price, level] : priceLevels) {
            fn(price, level.getTotalQuantity());
        }
    }

    PriceLevel* bestPriceLevel() {
        if (priceLevels.empty()) {
            return nullptr;
        }
        return &priceLevels.begin()->second;
    }

    void reduceQuantity(Price price, double quantity) {
        if (priceLevels.find(price) == priceLevels.end()) {
            throw std::invalid_argument("Price level not found");
        }
        priceLevels[price].reduceQuantity(quantity);
    }

    Price bestPrice() {
        if (priceLevels.empty()) {
            return 0;
        }
        return priceLevels.begin()->first;
    }
};
