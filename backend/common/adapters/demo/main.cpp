#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "SimulatedAdapter.h"
#include "Side.h"

#include <iostream>

int main() {
    EventDispatcher dispatcher;
    DeltaEngine deltaEngine;
    dispatcher.subscribe(&deltaEngine);

    SimulatedAdapter nseFeed(
        dispatcher,
        Exchange::NSE,
        {
            {"RELIANCE", 2950.0, 100.0, Side::BUY},
            {"RELIANCE", 2950.5, 40.0, Side::BUY},
            {"RELIANCE", 2951.0, 70.0, Side::SELL},
            {"RELIANCE", 2950.0, 30.0, Side::SELL},
        });

    std::cout << "ATHENA Adapter Demo\n";
    std::cout << "SimulatedAdapter -> EventDispatcher -> DeltaEngine\n\n";

    nseFeed.start();

    std::cout << "Buy volume:  " << deltaEngine.getBuyVolume() << "\n";
    std::cout << "Sell volume: " << deltaEngine.getSellVolume() << "\n";
    std::cout << "Net delta:   " << deltaEngine.getDelta() << "\n";

    return 0;
}
