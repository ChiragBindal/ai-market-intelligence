#include "Trade.h"
#include "IdGenerator.h"

#include <string>

Trade::Trade(int buyOrderId, int sellOrderId, Symbol symbol, double price, double quantity, Side aggressorSide)
    : Trade(buyOrderId, sellOrderId, std::move(symbol), price, quantity, aggressorSide, std::chrono::system_clock::now()) {}

Trade::Trade(int buyOrderId, int sellOrderId, Symbol symbol, double price, double quantity, Side aggressorSide,
             std::chrono::system_clock::time_point tradeTime)
    : tradeId(getIdByGenerator()),
      buyOrderId(buyOrderId),
      sellOrderId(sellOrderId),
      symbol(std::move(symbol)),
      price(price),
      quantity(quantity),
      tradeTime(tradeTime),
      aggressorSide(aggressorSide) {}

int Trade::getTradeId() const {
    return tradeId;
}

int Trade::getBuyOrderId() const {
    return buyOrderId;
}

int Trade::getSellOrderId() const {
    return sellOrderId;
}

Symbol Trade::getSymbol() const {
    return symbol;
}

double Trade::getPrice() const {
    return price;
}

double Trade::getQuantity() const {
    return quantity;
}

std::chrono::system_clock::time_point Trade::getTradeTime() const {
    return tradeTime;
}

Side Trade::getAggressorSide() const {
    return aggressorSide;
}

std::string Trade::toString() const {
    std::string aggressor = (aggressorSide == Side::BUY) ? "BUY" : "SELL";
    return "Trade [tradeId=" + std::to_string(tradeId)
        + ", buyOrderId=" + std::to_string(buyOrderId)
        + ", sellOrderId=" + std::to_string(sellOrderId)
        + ", symbol=" + symbol
        + ", price=" + std::to_string(price)
        + ", quantity=" + std::to_string(quantity)
        + ", tradeTime=" + std::to_string(tradeTime.time_since_epoch().count())
        + ", aggressorSide=" + aggressor + "]";
}
