#include "AbsorptionEngine.h"
#include "EventDispatcher.h"
#include "FootPrintEngine.h"
#include "ImbalanceEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

#include <chrono>

void runAbsorptionEngineTests(TestRunner& runner) {
    const auto now = std::chrono::system_clock::now();

    runner.run("AbsorptionEngine -> no absorption without footprint data", [&]() {
        FootPrintEngine footPrintEngine;
        ImbalanceEngine imbalanceEngine(footPrintEngine);
        AbsorptionEngine absorptionEngine(footPrintEngine, imbalanceEngine, 100.0);

        EXPECT_TRUE(runner, !absorptionEngine.isBuyAbsorption(100.0), "no buy absorption");
        EXPECT_TRUE(runner, !absorptionEngine.isSellAbsorption(100.0), "no sell absorption");
    });

    runner.run("AbsorptionEngine -> no absorption when next level exists", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 500, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 10, 101.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        AbsorptionEngine absorptionEngine(footPrintEngine, imbalanceEngine, 100.0);

        EXPECT_TRUE(runner, imbalanceEngine.isBuyImbalance(100.0), "buy imbalance present");
        EXPECT_TRUE(runner, !absorptionEngine.isSellAbsorption(100.0), "not top level for sell absorption");
    });

    runner.run("AbsorptionEngine -> no absorption when prev level exists", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::SELL, 500, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::BUY, 10, 99.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        AbsorptionEngine absorptionEngine(footPrintEngine, imbalanceEngine, 100.0);

        EXPECT_TRUE(runner, imbalanceEngine.isSellImbalance(100.0), "sell imbalance present");
        EXPECT_TRUE(runner, !absorptionEngine.isBuyAbsorption(100.0), "not bottom level for buy absorption");
    });

    runner.run("AbsorptionEngine -> volume threshold blocks absorption", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 50, 100.0, now);

        ImbalanceEngine imbalanceEngine(footPrintEngine, 3.0);
        AbsorptionEngine absorptionEngine(footPrintEngine, imbalanceEngine, 100.0);

        EXPECT_TRUE(runner, !absorptionEngine.isBuyAbsorption(100.0), "volume below threshold");
        EXPECT_TRUE(runner, !absorptionEngine.isSellAbsorption(100.0), "single level book");
    });
}
