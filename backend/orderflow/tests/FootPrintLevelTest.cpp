#include "FootPrintLevel.h"
#include "TestHelpers.h"
#include "TestRunner.h"

void runFootPrintLevelTests(TestRunner& runner) {
    runner.run("FootPrintLevel - volumes and delta", [&]() {
        FootPrintLevel level;

        EXPECT_TRUE(runner, level.processTrade(makeTrade(Side::BUY, 30, 100.0)), "accepts buy 30");
        EXPECT_TRUE(runner, level.processTrade(makeTrade(Side::SELL, 10, 100.0)), "accepts sell 10");
        EXPECT_TRUE(runner, level.processTrade(makeTrade(Side::BUY, 20, 100.0)), "accepts buy 20");

        EXPECT_EQUAL(runner, level.getBuyVolume(), 50.0, "buy volume at level");
        EXPECT_EQUAL(runner, level.getSellVolume(), 10.0, "sell volume at level");
        EXPECT_EQUAL(runner, level.getDelta(), 40.0, "delta at level");
        EXPECT_EQUAL(runner, level.getTotalVolume(), 60.0, "total volume at level");
        EXPECT_TRUE(runner, level.getTradeCount() == 3, "trade count");
        EXPECT_EQUAL(runner, level.getMaxPrintSize(), 30.0, "max print size");
    });

    runner.run("FootPrintLevel - soft skip invalid quantity", [&]() {
        FootPrintLevel level;
        level.processTrade(makeTrade(Side::BUY, 50, 100.0));

        EXPECT_TRUE(runner, !level.processTrade(makeTrade(Side::BUY, 0, 100.0)), "skips zero qty");
        EXPECT_TRUE(runner, !level.processTrade(makeTrade(Side::SELL, -5, 100.0)), "skips negative qty");

        EXPECT_EQUAL(runner, level.getBuyVolume(), 50.0, "state unchanged");
        EXPECT_TRUE(runner, level.getTradeCount() == 1, "trade count unchanged");
    });

    runner.run("FootPrintLevel - reset", [&]() {
        FootPrintLevel level;
        level.processTrade(makeTrade(Side::BUY, 40, 100.0));
        level.reset();

        EXPECT_EQUAL(runner, level.getBuyVolume(), 0.0, "reset buy");
        EXPECT_EQUAL(runner, level.getSellVolume(), 0.0, "reset sell");
        EXPECT_EQUAL(runner, level.getDelta(), 0.0, "reset delta");
        EXPECT_TRUE(runner, level.getTradeCount() == 0, "reset trade count");
        EXPECT_EQUAL(runner, level.getMaxPrintSize(), 0.0, "reset max print");
    });
}
