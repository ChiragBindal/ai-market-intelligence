#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "ExhaustionEngine.h"
#include "FootPrintEngine.h"
#include "TradeEvent.h"
#include "TestHelpers.h"
#include "TestRunner.h"

#include <chrono>

void runEventDispatcherTests(TestRunner& runner) {
    const auto now = std::chrono::system_clock::now();

    runner.run("EventDispatcher -> DeltaEngine", [&]() {
        EventDispatcher dispatcher;
        DeltaEngine engine;
        dispatcher.subscribe(&engine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 100, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 40, 100.0, now);
        publishTrade(dispatcher, 3, 3, Side::BUY, 60, 101.0, now);
        publishTrade(dispatcher, 4, 4, Side::SELL, 20, 101.0, now);

        EXPECT_EQUAL(runner, engine.getBuyVolume(), 160.0, "buyVolume updated");
        EXPECT_EQUAL(runner, engine.getSellVolume(), 60.0, "sellVolume updated");
        EXPECT_EQUAL(runner, engine.getDelta(), 100.0, "delta updated");
    });

    runner.run("EventDispatcher -> FootPrintEngine", [&]() {
        EventDispatcher dispatcher;
        FootPrintEngine engine;
        dispatcher.subscribe(&engine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 100, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 40, 101.0, now);
        publishTrade(dispatcher, 3, 3, Side::BUY, 60, 100.0, now);
        publishTrade(dispatcher, 4, 4, Side::SELL, 20, 101.0, now);

        const FootPrintLevel* level100 = engine.getLevel(100.0);
        const FootPrintLevel* level101 = engine.getLevel(101.0);

        EXPECT_TRUE(runner, level100 != nullptr, "level 100 created");
        EXPECT_TRUE(runner, level101 != nullptr, "level 101 created");
        EXPECT_EQUAL(runner, level100->getBuyVolume(), 160.0, "100 buy volume");
        EXPECT_EQUAL(runner, level100->getSellVolume(), 0.0, "100 sell volume");
        EXPECT_EQUAL(runner, level100->getDelta(), 160.0, "100 delta");
        EXPECT_EQUAL(runner, level101->getBuyVolume(), 0.0, "101 buy volume");
        EXPECT_EQUAL(runner, level101->getSellVolume(), 60.0, "101 sell volume");
        EXPECT_EQUAL(runner, level101->getDelta(), -60.0, "101 delta");
    });

    runner.run("EventDispatcher -> ExhaustionEngine buy exhaustion", [&]() {
        EventDispatcher dispatcher;
        ExhaustionEngine engine(5, 0.10, 3);
        dispatcher.subscribe(&engine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 5000, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::BUY, 4200, 100.0, now);
        publishTrade(dispatcher, 3, 3, Side::BUY, 3400, 100.0, now);
        publishTrade(dispatcher, 4, 4, Side::BUY, 2500, 100.0, now);
        publishTrade(dispatcher, 5, 5, Side::BUY, 1800, 100.0, now);

        EXPECT_TRUE(runner, engine.isBuyExhaustion(), "buy exhaustion via onEvent");
        EXPECT_TRUE(runner, !engine.isSellExhaustion(), "sell not exhausted");
    });

    runner.run("EventDispatcher -> ExhaustionEngine sell exhaustion", [&]() {
        EventDispatcher dispatcher;
        ExhaustionEngine engine(5, 0.10, 3);
        dispatcher.subscribe(&engine);

        publishTrade(dispatcher, 1, 1, Side::SELL, 6000, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 5000, 100.0, now);
        publishTrade(dispatcher, 3, 3, Side::SELL, 4200, 100.0, now);
        publishTrade(dispatcher, 4, 4, Side::SELL, 3000, 100.0, now);
        publishTrade(dispatcher, 5, 5, Side::SELL, 1800, 100.0, now);

        EXPECT_TRUE(runner, engine.isSellExhaustion(), "sell exhaustion via onEvent");
        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "buy not exhausted");
    });

    runner.run("EventDispatcher -> all AnalysisEngines together", [&]() {
        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        FootPrintEngine footPrintEngine;
        ExhaustionEngine exhaustionEngine(5, 0.10, 3);

        dispatcher.subscribe(&deltaEngine);
        dispatcher.subscribe(&footPrintEngine);
        dispatcher.subscribe(&exhaustionEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 100, 100.0, now);
        publishTrade(dispatcher, 2, 2, Side::SELL, 40, 100.0, now);
        publishTrade(dispatcher, 3, 3, Side::BUY, 5000, 101.0, now);
        publishTrade(dispatcher, 4, 4, Side::BUY, 4200, 101.0, now);
        publishTrade(dispatcher, 5, 5, Side::BUY, 3400, 101.0, now);
        publishTrade(dispatcher, 6, 6, Side::BUY, 2500, 101.0, now);
        publishTrade(dispatcher, 7, 7, Side::BUY, 1800, 101.0, now);

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 17000.0, "delta engine buy volume");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 40.0, "delta engine sell volume");
        EXPECT_EQUAL(runner, footPrintEngine.getLevel(100.0)->getDelta(), 60.0, "footprint 100 delta");
        EXPECT_TRUE(runner, exhaustionEngine.isBuyExhaustion(), "exhaustion from later buy sequence");
    });

    runner.run("EventDispatcher -> engine reset", [&]() {
        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        FootPrintEngine footPrintEngine;
        dispatcher.subscribe(&deltaEngine);
        dispatcher.subscribe(&footPrintEngine);

        publishTrade(dispatcher, 1, 1, Side::BUY, 100, 100.0, now);
        deltaEngine.reset();
        footPrintEngine.reset();

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 0.0, "delta reset buy volume");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 0.0, "delta reset sell volume");
        EXPECT_TRUE(runner, footPrintEngine.getLevel(100.0) == nullptr, "footprint reset clears levels");
    });
}
