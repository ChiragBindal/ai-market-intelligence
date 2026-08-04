#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "FootPrintEngine.h"
#include "BinanceAdapter.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

// Prints what both engines learned from the same Binance trade stream.
static void printAnalytics(const DeltaEngine& delta, const FootPrintEngine& footprint) {
    std::cout << "\n--- DeltaEngine (session aggressor pressure) ---\n";
    std::cout << "Buy volume:  " << delta.getBuyVolume() << "\n";
    std::cout << "Sell volume: " << delta.getSellVolume() << "\n";
    std::cout << "Net delta:   " << delta.getDelta() << "\n";
    std::cout << "Delta %:     " << delta.getDeltaPct() * 100.0 << "%\n";

    const FootPrintSnapshot snap = footprint.snapshot();
    std::cout << "\n--- FootPrintEngine (volume by price level) ---\n";
    std::cout << "Price levels: " << snap.levelCount << "\n";
    std::cout << "Low price:    " << snap.lowPrice << "\n";
    std::cout << "High price:   " << snap.highPrice << "\n";
    std::cout << "Buy volume:   " << snap.totalBuyVolume << "\n";
    std::cout << "Sell volume:  " << snap.totalSellVolume << "\n";
    std::cout << "Net delta:    " << snap.totalDelta << "\n";
    std::cout << "Total volume: " << snap.totalVolume << "\n";

    // Market principle: footprint delta should match session delta when both
    // engines saw the same trades (tick grouping does not drop quantity).
    const bool match = std::abs(delta.getDelta() - snap.totalDelta) < 1e-9;
    std::cout << "\nDelta vs Footprint delta match: " << (match ? "YES" : "CHECK") << "\n";
}

int main(int argc, char** argv) {
    // Usage:
    //   binance_stream_demo.exe              -> WebSocket stream (default)
    //   binance_stream_demo.exe rest         -> REST snapshot
    //   binance_stream_demo.exe stream 20 80 -> stream 20s / max 80 trades
    const bool useRest = (argc >= 2 && std::string(argv[1]) == "rest");
    const int durationSeconds = (argc >= 3) ? std::atoi(argv[2]) : 15;
    const int maxTrades = (argc >= 4) ? std::atoi(argv[3]) : 100;

    EventDispatcher dispatcher;

    // Both engines subscribe to the SAME event bus — one trade, many listeners.
    DeltaEngine deltaEngine;
    // BTCUSDT price tick is typically 0.01 on Binance spot.
    FootPrintEngine footPrintEngine(0.01);
    dispatcher.subscribe(&deltaEngine);
    dispatcher.subscribe(&footPrintEngine);

    BinanceAdapter binanceFeed(dispatcher, "BTCUSDT", 50);

    std::cout << "ATHENA Binance Live Analytics Demo\n";
    std::cout << "Feed -> TradeEvent -> EventDispatcher -> DeltaEngine + FootPrintEngine\n\n";

    if (useRest) {
        std::cout << "Mode: REST snapshot\n";
        binanceFeed.start();
    } else {
        std::cout << "Mode: WebSocket stream (" << durationSeconds << "s, max "
                  << maxTrades << " trades)\n";
        const int n = binanceFeed.startStreaming(durationSeconds, maxTrades);
        if (n <= 0) {
            std::cout << "No WebSocket trades. Falling back to REST snapshot...\n";
            binanceFeed.stop();
            binanceFeed.start();
        }
    }

    if (deltaEngine.getTotalVolume() <= 0.0) {
        std::cout << "No trades processed. Check internet / PowerShell / curl.exe.\n";
        return 1;
    }

    printAnalytics(deltaEngine, footPrintEngine);
    return 0;
}
