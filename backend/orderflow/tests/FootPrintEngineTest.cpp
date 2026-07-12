#include "FootPrintEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

void runFootPrintEngineTests(TestRunner& runner) {
    runner.run("FootPrintEngine", [&]() {
        FootPrintEngine engine;

        engine.processTrade(makeTrade(Side::BUY, 100, 100.0));
        engine.processTrade(makeTrade(Side::SELL, 40, 101.0));
        engine.processTrade(makeTrade(Side::BUY, 60, 100.0));
        engine.processTrade(makeTrade(Side::SELL, 20, 101.0));

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

        engine.reset();
        EXPECT_TRUE(runner, engine.getLevel(100.0) == nullptr, "reset clears levels");
    });
}
