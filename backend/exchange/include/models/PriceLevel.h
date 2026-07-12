#pragma once

#include <unordered_map>
#include <iostream>
#include "Order.h"
#include "DoubleLL.h"

using OrderId = int;

class PriceLevel {
private:
    double totalQuantity;
    DLL<Order> orders;
    std::unordered_map<OrderId, Node<Order>*> orderMap;

public:
    PriceLevel();
    bool addOrder(Order* order);
    Order* getFrontOrder();
    int getOrderCount();
    double getTotalQuantity() const;
    DLL<Order>& getOrders();
    void removeOrder(int orderId);
    void reduceQuantity(double quantity);
};
