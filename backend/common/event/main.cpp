#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "TradeEvent.h"

#include <chrono>
#include <iostream>

int main() {
    EventDispatcher dispatcher;
    DeltaEngine deltaEngine;
    dispatcher.subscribe(&deltaEngine);

    const auto now = std::chrono::system_clock::now();

    // Create Trade
    Trade buyTrade1(1, 2, "AAPL", 100.0, 100.0, Side::BUY, now);
    Trade sellTrade(3, 4, "AAPL", 100.0, 40.0, Side::SELL, now);
    Trade buyTrade2(5, 6, "AAPL", 101.0, 60.0, Side::BUY, now);
    Trade sellTrade2(7, 8, "AAPL", 101.0, 20.0, Side::SELL, now);

    // Create TradeEvent
    TradeEvent buyEvent1(1, 1, Exchange::NSE, buyTrade1);
    TradeEvent sellEvent1(2, 2, Exchange::NSE, sellTrade);
    TradeEvent buyEvent2(3, 3, Exchange::NSE, buyTrade2);
    TradeEvent sellEvent2(4, 4, Exchange::NSE, sellTrade2);

    // dispatcher.publish()
    std::cout << "Create Trade -> Create TradeEvent -> dispatcher.publish()" << std::endl;
    dispatcher.publish(buyEvent1);
    dispatcher.publish(sellEvent1);
    dispatcher.publish(buyEvent2);
    dispatcher.publish(sellEvent2);

    // Verify
    std::cout << "Buy Volume  = " << deltaEngine.getBuyVolume() << std::endl;
    std::cout << "Sell Volume = " << deltaEngine.getSellVolume() << std::endl;
    std::cout << "Delta       = " << deltaEngine.getDelta() << std::endl;

    return 0;
}
