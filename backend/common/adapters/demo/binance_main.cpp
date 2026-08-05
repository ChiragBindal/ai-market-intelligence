#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "FootPrintEngine.h"
#include "BinanceAdapter.h"

#include <cmath>
#include <iostream>

int main() {
    EventDispatcher dispatcher;
    DeltaEngine deltaEngine;
    FootPrintEngine footPrintEngine(0.01);

    // One EventDispatcher, two AnalysisEngines — Observer pattern.
    dispatcher.subscribe(&deltaEngine);
    dispatcher.subscribe(&footPrintEngine);

    BinanceAdapter binanceFeed(dispatcher, "BTCUSDT", 50);

    std::cout << "ATHENA Binance REST Demo\n";
    std::cout << "REST -> TradeEvent -> DeltaEngine + FootPrintEngine\n\n";

    binanceFeed.start();

    if (deltaEngine.getTotalVolume() <= 0.0) {
        std::cout << "No trades processed. Check internet connection and try again.\n";
        return 1;
    }

    std::cout << "--- DeltaEngine ---\n";
    std::cout << "Buy volume:  " << deltaEngine.getBuyVolume() << "\n";
    std::cout << "Sell volume: " << deltaEngine.getSellVolume() << "\n";
    std::cout << "Net delta:   " << deltaEngine.getDelta() << "\n";
    std::cout << "Delta %:     " << deltaEngine.getDeltaPct() * 100.0 << "%\n";

    const FootPrintSnapshot snap = footPrintEngine.snapshot();
    std::cout << "\n--- FootPrintEngine ---\n";
    std::cout << "Price levels: " << snap.levelCount << "\n";
    std::cout << "Range:        " << snap.lowPrice << " -> " << snap.highPrice << "\n";
    std::cout << "Buy volume:   " << snap.totalBuyVolume << "\n";
    std::cout << "Sell volume:  " << snap.totalSellVolume << "\n";
    std::cout << "Net delta:    " << snap.totalDelta << "\n";

    const bool match = std::abs(deltaEngine.getDelta() - snap.totalDelta) < 1e-9;
    std::cout << "\nDelta match:  " << (match ? "YES" : "CHECK") << "\n";

    return 0;
}
