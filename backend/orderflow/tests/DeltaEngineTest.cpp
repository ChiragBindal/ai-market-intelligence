#include "DeltaEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

void runDeltaEngineTests(TestRunner& runner) {
    runner.run("DeltaEngine - basic cumulative", [&]() {
        DeltaEngine engine;

        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 100)), "accepts buy 100");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::SELL, 40)), "accepts sell 40");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 60)), "accepts buy 60");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::SELL, 20)), "accepts sell 20");

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 160.0, "buy volume");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 60.0, "sell volume");
        EXPECT_EQUAL(runner, engine.getDelta(), 100.0, "delta");
        EXPECT_EQUAL(runner, engine.getTotalVolume(), 220.0, "total volume");
        EXPECT_EQUAL(runner, engine.getDeltaPct(), 100.0 / 220.0, "delta pct");
    });

    runner.run("DeltaEngine - snapshot for fusion", [&]() {
        DeltaEngine engine;
        engine.processTrade(makeTrade(Side::BUY, 100));
        engine.processTrade(makeTrade(Side::SELL, 40));

        const DeltaSnapshot snap = engine.snapshot();
        EXPECT_EQUAL(runner, snap.buyVolume, 100.0, "snapshot buy");
        EXPECT_EQUAL(runner, snap.sellVolume, 40.0, "snapshot sell");
        EXPECT_EQUAL(runner, snap.delta, 60.0, "snapshot delta");
        EXPECT_EQUAL(runner, snap.totalVolume, 140.0, "snapshot total");
        EXPECT_EQUAL(runner, snap.deltaPct, 60.0 / 140.0, "snapshot deltaPct");
    });

    runner.run("DeltaEngine - soft skip invalid quantity", [&]() {
        DeltaEngine engine;
        engine.processTrade(makeTrade(Side::BUY, 50));

        EXPECT_TRUE(runner, !engine.processTrade(makeTrade(Side::BUY, 0)), "skips zero qty");
        EXPECT_TRUE(runner, !engine.processTrade(makeTrade(Side::SELL, -10)), "skips negative qty");

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 50.0, "state unchanged after skip");
        EXPECT_EQUAL(runner, engine.getDelta(), 50.0, "delta unchanged after skip");
    });

    runner.run("DeltaEngine - symbol filter", [&]() {
        DeltaEngine engine(0, "AAPL");

        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 100, 100.0)), "accepts AAPL");
        Trade other(1, 2, "MSFT", 200.0, 80.0, Side::BUY);
        EXPECT_TRUE(runner, !engine.processTrade(other), "rejects other symbol");

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 100.0, "only AAPL counted");
        EXPECT_TRUE(runner, engine.getSymbolFilter() == "AAPL", "filter stored");
    });

    runner.run("DeltaEngine - rolling window", [&]() {
        DeltaEngine engine(3);

        engine.processTrade(makeTrade(Side::BUY, 100));   // window: +100
        engine.processTrade(makeTrade(Side::SELL, 40));   // window: +100, -40
        engine.processTrade(makeTrade(Side::BUY, 60));    // window: +100, -40, +60
        EXPECT_EQUAL(runner, engine.getBuyVolume(), 160.0, "full window buy");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 40.0, "full window sell");
        EXPECT_EQUAL(runner, engine.getDelta(), 120.0, "full window delta");
        EXPECT_TRUE(runner, engine.getObservationCount() == 3, "window size 3");

        // Drop oldest buy 100 → remaining sell 40 + buy 60
        engine.processTrade(makeTrade(Side::SELL, 20));
        EXPECT_EQUAL(runner, engine.getBuyVolume(), 60.0, "rolled buy volume");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 60.0, "rolled sell volume");
        EXPECT_EQUAL(runner, engine.getDelta(), 0.0, "rolled delta");
        EXPECT_TRUE(runner, engine.getObservationCount() == 3, "window stays at 3");
    });

    runner.run("DeltaEngine - reset", [&]() {
        DeltaEngine engine(5);
        engine.processTrade(makeTrade(Side::BUY, 100));
        engine.processTrade(makeTrade(Side::SELL, 40));
        engine.reset();

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 0.0, "reset buy volume");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 0.0, "reset sell volume");
        EXPECT_EQUAL(runner, engine.getDelta(), 0.0, "reset delta");
        EXPECT_EQUAL(runner, engine.getDeltaPct(), 0.0, "reset deltaPct");
        EXPECT_TRUE(runner, engine.getObservationCount() == 0, "reset clears observations");
    });
}
