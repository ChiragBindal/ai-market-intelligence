#include "FootPrintEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

void runFootPrintEngineTests(TestRunner& runner) {
    runner.run("FootPrintEngine - basic levels", [&]() {
        FootPrintEngine engine(1.0);

        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 100, 100.0)), "buy @100");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::SELL, 40, 101.0)), "sell @101");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 60, 100.0)), "buy @100");
        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::SELL, 20, 101.0)), "sell @101");

        const FootPrintLevel* level100 = engine.getLevel(100.0);
        const FootPrintLevel* level101 = engine.getLevel(101.0);
        const FootPrintLevel* missing = engine.getLevel(99.0);

        EXPECT_TRUE(runner, level100 != nullptr, "creates level at 100");
        EXPECT_TRUE(runner, level101 != nullptr, "creates level at 101");
        EXPECT_TRUE(runner, missing == nullptr, "missing level returns nullptr");

        EXPECT_EQUAL(runner, level100->getBuyVolume(), 160.0, "100 buy volume");
        EXPECT_EQUAL(runner, level100->getSellVolume(), 0.0, "100 sell volume");
        EXPECT_EQUAL(runner, level100->getDelta(), 160.0, "100 delta");

        EXPECT_EQUAL(runner, level101->getBuyVolume(), 0.0, "101 buy volume");
        EXPECT_EQUAL(runner, level101->getSellVolume(), 60.0, "101 sell volume");
        EXPECT_EQUAL(runner, level101->getDelta(), -60.0, "101 delta");
        EXPECT_TRUE(runner, engine.getLevelCount() == 2, "two levels");
    });

    runner.run("FootPrintEngine - tick rounding", [&]() {
        FootPrintEngine engine(0.05);

        engine.processTrade(makeTrade(Side::BUY, 10, 100.00));
        engine.processTrade(makeTrade(Side::BUY, 20, 100.02));  // rounds to 100.00
        engine.processTrade(makeTrade(Side::SELL, 5, 100.049)); // rounds to 100.05

        EXPECT_TRUE(runner, engine.getLevel(100.00) != nullptr, "level at 100.00");
        EXPECT_TRUE(runner, engine.getLevel(100.05) != nullptr, "level at 100.05");
        EXPECT_EQUAL(runner, engine.getLevel(100.00)->getBuyVolume(), 30.0, "merged into 100.00");
        EXPECT_EQUAL(runner, engine.getLevel(100.05)->getSellVolume(), 5.0, "rounded to 100.05");
        EXPECT_TRUE(runner, engine.getLevelCount() == 2, "two tick levels");
    });

    runner.run("FootPrintEngine - prev/next ordering", [&]() {
        FootPrintEngine engine(1.0);
        engine.processTrade(makeTrade(Side::BUY, 10, 99.0));
        engine.processTrade(makeTrade(Side::BUY, 20, 100.0));
        engine.processTrade(makeTrade(Side::BUY, 30, 101.0));

        EXPECT_TRUE(runner, engine.getPrevLevel(100.0) == engine.getLevel(99.0), "prev is lower price");
        EXPECT_TRUE(runner, engine.getNextLevel(100.0) == engine.getLevel(101.0), "next is higher price");
        EXPECT_TRUE(runner, engine.getPrevLevel(99.0) == nullptr, "no prev at low");
        EXPECT_TRUE(runner, engine.getNextLevel(101.0) == nullptr, "no next at high");
    });

    runner.run("FootPrintEngine - snapshot for fusion", [&]() {
        FootPrintEngine engine(1.0);
        engine.processTrade(makeTrade(Side::BUY, 100, 100.0));
        engine.processTrade(makeTrade(Side::SELL, 40, 101.0));
        engine.processTrade(makeTrade(Side::BUY, 60, 100.0));

        const FootPrintSnapshot snap = engine.snapshot();
        EXPECT_EQUAL(runner, snap.lowPrice, 100.0, "snapshot low");
        EXPECT_EQUAL(runner, snap.highPrice, 101.0, "snapshot high");
        EXPECT_EQUAL(runner, snap.totalBuyVolume, 160.0, "snapshot buy");
        EXPECT_EQUAL(runner, snap.totalSellVolume, 40.0, "snapshot sell");
        EXPECT_EQUAL(runner, snap.totalDelta, 120.0, "snapshot delta");
        EXPECT_EQUAL(runner, snap.totalVolume, 200.0, "snapshot total");
        EXPECT_TRUE(runner, snap.levelCount == 2, "snapshot level count");
    });

    runner.run("FootPrintEngine - symbol filter and soft skip", [&]() {
        FootPrintEngine engine(1.0, "AAPL");

        EXPECT_TRUE(runner, engine.processTrade(makeTrade(Side::BUY, 50, 100.0)), "accepts AAPL");
        Trade other(1, 2, "MSFT", 100.0, 80.0, Side::BUY);
        EXPECT_TRUE(runner, !engine.processTrade(other), "rejects other symbol");
        EXPECT_TRUE(runner, !engine.processTrade(makeTrade(Side::BUY, 0, 100.0)), "skips zero qty");

        EXPECT_EQUAL(runner, engine.getLevel(100.0)->getBuyVolume(), 50.0, "only AAPL counted");
        EXPECT_TRUE(runner, engine.getSymbolFilter() == "AAPL", "filter stored");
    });

    runner.run("FootPrintEngine - reset", [&]() {
        FootPrintEngine engine(1.0);
        engine.processTrade(makeTrade(Side::BUY, 100, 100.0));
        engine.reset();

        EXPECT_TRUE(runner, engine.getLevel(100.0) == nullptr, "reset clears levels");
        EXPECT_TRUE(runner, engine.getLevelCount() == 0, "level count zero");
        EXPECT_EQUAL(runner, engine.snapshot().totalVolume, 0.0, "empty snapshot");
    });
}
