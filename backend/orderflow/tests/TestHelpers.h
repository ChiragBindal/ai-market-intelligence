#pragma once

#include "Trade.h"
#include "TradeEvent.h"
#include "EventDispatcher.h"
#include "Side.h"

#include <chrono>

inline Trade makeTrade(Side aggressorSide, double quantity, double price = 100.0) {
    return Trade(1, 2, "AAPL", price, quantity, aggressorSide);
}

inline Trade makeTradeAt(Side aggressorSide, double quantity, double price,
                         std::chrono::system_clock::time_point tradeTime) {
    return Trade(1, 2, "AAPL", price, quantity, aggressorSide, tradeTime);
}

inline bool approxEqual(double a, double b, double epsilon = 1e-9) {
    return (a > b ? a - b : b - a) < epsilon;
}

inline void publishTrade(EventDispatcher& dispatcher,
                         EventId eventId,
                         SequenceNumber sequence,
                         Side aggressorSide,
                         double quantity,
                         double price,
                         std::chrono::system_clock::time_point tradeTime = std::chrono::system_clock::now()) {
    Trade trade = makeTradeAt(aggressorSide, quantity, price, tradeTime);
    TradeEvent event(eventId, sequence, Exchange::NSE, trade);
    dispatcher.publish(event);
}
