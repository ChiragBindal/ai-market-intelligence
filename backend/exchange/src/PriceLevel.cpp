#include "PriceLevel.h"
#include <iostream>
#include <stdexcept>

PriceLevel::PriceLevel() : totalQuantity(0), orders() {}

bool PriceLevel::addOrder(Order* order) {
    if (order == nullptr) {
        throw std::invalid_argument("Order cannot be nullptr");
    }
    if (order->getRemainingQuantity() <= 0) {
        throw std::invalid_argument("Order quantity must be greater than 0");
    }
    orders.insertAtTail(order);
    orderMap[order->getOrderId()] = orders.getTail();
    totalQuantity += order->getRemainingQuantity();
    return true;
}

Order* PriceLevel::getFrontOrder() {
    if (orders.isEmpty()) {
        std::cout << "No orders in the price level" << std::endl;
        throw std::runtime_error("No orders in the price level");
    }
    return orders.getHead()->data;
}

int PriceLevel::getOrderCount() {
    return orders.getSize();
}

double PriceLevel::getTotalQuantity() const {
    return totalQuantity;
}

DLL<Order>& PriceLevel::getOrders() {
    return orders;
}

void PriceLevel::removeOrder(int orderId) {
    if (orders.isEmpty()) {
        std::cout << "No orders in the price level" << std::endl;
        throw std::runtime_error("Price level is empty");
    }
    auto it = orderMap.find(orderId);
    if (it == orderMap.end() || it->second == nullptr) {
        throw std::invalid_argument("Order not found in the price level");
    }
    Node<Order>* orderNode = it->second;
    totalQuantity -= orderNode->data->getRemainingQuantity();
    orders.removeNode(orderNode);
    orderMap.erase(orderId);
}

void PriceLevel::reduceQuantity(double quantity) {
    if (quantity <= 0) {
        throw std::invalid_argument("Quantity must be greater than 0");
    }
    if (quantity > totalQuantity) {
        throw std::invalid_argument("Quantity must be less than the total quantity");
    }
    totalQuantity -= quantity;
}
