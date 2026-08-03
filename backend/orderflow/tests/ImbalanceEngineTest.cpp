#include "EventDispatcher.h"
#include "FootPrintEngine.h"
#include "ImbalanceEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

#include <chrono>

void runImbalanceEngineTests(TestRunner& runner) {
    const auto now = std::chrono::system_clock::now();

    runner.run("ImbalanceEngine -> buy imbalance", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 300, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 50, 101.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        EXPECT_TRUE(runner, imbalanceEngine.isBuyImbalance(100.0), "buy imbalance at 100");
        EXPECT_TRUE(runner, !imbalanceEngine.isSellImbalance(100.0), "no sell imbalance at 100");
    });

    runner.run("ImbalanceEngine -> sell imbalance", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::SELL, 300, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::BUY, 50, 99.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        EXPECT_TRUE(runner, imbalanceEngine.isSellImbalance(100.0), "sell imbalance at 100");
        EXPECT_TRUE(runner, !imbalanceEngine.isBuyImbalance(100.0), "no buy imbalance at 100");
    });

    runner.run("ImbalanceEngine -> no imbalance when ratio not met", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 100, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 50, 101.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        EXPECT_TRUE(runner, !imbalanceEngine.isBuyImbalance(100.0), "100 not greater than 3*50");
        EXPECT_TRUE(runner, !imbalanceEngine.isBuyImbalance(101.0), "missing next level at 101");
    });
}
