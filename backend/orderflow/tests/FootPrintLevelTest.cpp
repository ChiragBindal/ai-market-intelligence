#include "FootPrintLevel.h"
#include "TestHelpers.h"
#include "TestRunner.h"

void runFootPrintLevelTests(TestRunner& runner) {
    runner.run("FootPrintLevel", [&]() {
        FootPrintLevel level;

        level.processTrade(makeTrade(Side::BUY, 30, 100.0));
        level.processTrade(makeTrade(Side::SELL, 10, 100.0));
        level.processTrade(makeTrade(Side::BUY, 20, 100.0));

        EXPECT_EQUAL(runner, level.getBuyVolume(), 50.0, "buy volume at level");
        EXPECT_EQUAL(runner, level.getSellVolume(), 10.0, "sell volume at level");
        EXPECT_EQUAL(runner, level.getDelta(), 40.0, "delta at level");
    });
}
