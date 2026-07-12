#include "DeltaEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

#include <stdexcept>

void runDeltaEngineTests(TestRunner& runner) {
    runner.run("DeltaEngine", [&]() {
        DeltaEngine engine;

        engine.processTrade(makeTrade(Side::BUY, 100));
        engine.processTrade(makeTrade(Side::SELL, 40));
        engine.processTrade(makeTrade(Side::BUY, 60));
        engine.processTrade(makeTrade(Side::SELL, 20));

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 160.0, "buy volume");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 60.0, "sell volume");
        EXPECT_EQUAL(runner, engine.getDelta(), 100.0, "delta");

        engine.reset();
        EXPECT_EQUAL(runner, engine.getBuyVolume(), 0.0, "reset buy volume");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 0.0, "reset sell volume");
        EXPECT_EQUAL(runner, engine.getDelta(), 0.0, "reset delta");

        bool threw = false;
        try {
            engine.processTrade(makeTrade(Side::BUY, 0));
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        EXPECT_TRUE(runner, threw, "rejects zero quantity");
    });
}
