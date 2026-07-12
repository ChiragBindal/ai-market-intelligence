#include "Order.h"
#include "IdGenerator.h"
#include <stdexcept>

Order::Order(std::string symbol, Side side, OrderType orderType, double price, double quantity)
    : orderId(getIdByGenerator()),
      symbol(std::move(symbol)),
      side(side),
      orderType(orderType),
      orderStatus(OrderStatus::ACCEPTED),
      price(price),
      originalQuantity(quantity),
      executedQuantity(0),
      remainingQuantity(quantity),
      createdAt(std::chrono::system_clock::now()) {
        if (quantity <= 0)
            throw std::invalid_argument("Quantity must be greater than zero");

        if (orderType == OrderType::LIMIT && price <= 0) 
            throw std::invalid_argument("Limit order price must be greater than zero");
      }

bool Order::isFilled() const {
    return orderStatus == OrderStatus::FILLED && remainingQuantity == 0;
}

bool Order::isCancelled() const {
    return orderStatus == OrderStatus::CANCELLED;
}

double Order::getRemainingQuantity() const {
    return remainingQuantity;
}

void Order::execute(double quantity) {
    if (isCancelled() || isFilled()) {
        throw std::runtime_error("Order is already cancelled or filled");
    }
    if (quantity <= 0) {
        throw std::runtime_error("Invalid order quantity");
    }

    if (remainingQuantity >= quantity) {
        executedQuantity += quantity;
        remainingQuantity -= quantity;
        if (remainingQuantity == 0) {
            orderStatus = OrderStatus::FILLED;
            filledAt = std::chrono::system_clock::now();
        } else {
            orderStatus = OrderStatus::PARTIALLY_FILLED;
        }
    } else {
        throw std::runtime_error("Insufficient quantity to fill the order");
    }
}

void Order::cancel() {
    if (isCancelled() || isFilled()) {
        throw std::runtime_error("Order is already cancelled or filled");
    }
    orderStatus = OrderStatus::CANCELLED;
}

double Order::getPrice() const {
    return price;
}

Side Order::getSide() const {
    return side;
}

OrderType Order::getOrderType() const {
    return orderType;
}

OrderStatus Order::getOrderStatus() const {
    return orderStatus;
}

std::string Order::getSymbol() const {
    return symbol;
}

int Order::getOrderId() const {
    return orderId;
}

double Order::getOriginalQuantity() const {
    return originalQuantity;
}

double Order::getExecutedQuantity() const {
    return executedQuantity;
}

std::chrono::system_clock::time_point Order::getCreatedAt() const {
    return createdAt;
}

std::chrono::system_clock::time_point Order::getFilledAt() const {
    return filledAt;
}