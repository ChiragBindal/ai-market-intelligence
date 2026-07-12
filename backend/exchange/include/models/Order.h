#pragma once

#include <string>
#include <chrono>
#include "Side.h"
#include "OrderType.h"
#include "OrderStatus.h"

class Order {
private:
    int orderId;
    std::string symbol;
    Side side;
    OrderType orderType;
    OrderStatus orderStatus;
    double price;
    double originalQuantity;
    double executedQuantity;
    double remainingQuantity;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point filledAt;

public:
    Order(std::string symbol, Side side, OrderType orderType, double price, double originalQuantity);
    bool isFilled() const;
    bool isCancelled() const;
    double getRemainingQuantity() const;
    void execute(double quantity);
    void cancel();
    double getPrice() const;
    Side getSide() const;
    OrderType getOrderType() const;
    OrderStatus getOrderStatus() const;
    std::string getSymbol() const;
    int getOrderId() const;
    double getOriginalQuantity() const;
    double getExecutedQuantity() const;
    std::chrono::system_clock::time_point getCreatedAt() const;
    std::chrono::system_clock::time_point getFilledAt() const;
};
