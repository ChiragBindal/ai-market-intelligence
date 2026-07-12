#pragma once

#include <string>
#include <chrono>
#include "Side.h"

using Symbol = std::string;

class Trade {
private:
    int tradeId;
    int buyOrderId;
    int sellOrderId;
    Symbol symbol;
    double price;
    double quantity;
    std::chrono::system_clock::time_point tradeTime;
    Side aggressorSide;

public:
    Trade(int buyOrderId, int sellOrderId, Symbol symbol, double price, double quantity, Side aggressorSide);
    int getTradeId() const;
    int getBuyOrderId() const;
    int getSellOrderId() const;
    Symbol getSymbol() const;
    double getPrice() const;
    double getQuantity() const;
    std::chrono::system_clock::time_point getTradeTime() const;
    Side getAggressorSide() const;
    std::string toString() const;
};
