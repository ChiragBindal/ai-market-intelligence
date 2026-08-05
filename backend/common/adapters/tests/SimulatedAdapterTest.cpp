#include "EventDispatcher.h"
#include "DeltaEngine.h"
#include "SimulatedAdapter.h"
#include "Side.h"
#include "TestRunner.h"

#include <cmath>

void runSimulatedAdapterTests(TestRunner& runner) {
    runner.run("SimulatedAdapter -> EventDispatcher -> DeltaEngine", [&]() {
        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        dispatcher.subscribe(&deltaEngine);

        SimulatedAdapter adapter(
            dispatcher,
            Exchange::NSE,
            {
                {"RELIANCE", 2950.0, 100.0, Side::BUY},
                {"RELIANCE", 2950.5, 40.0, Side::BUY},
                {"RELIANCE", 2951.0, 70.0, Side::SELL},
                {"RELIANCE", 2950.0, 30.0, Side::SELL},
            });

        adapter.start();

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 140.0, "buy volume from adapter");
        EXPECT_EQUAL(runner, deltaEngine.getSellVolume(), 100.0, "sell volume from adapter");
        EXPECT_EQUAL(runner, deltaEngine.getDelta(), 40.0, "net delta from adapter");
        EXPECT_TRUE(runner, adapter.isRunning(), "adapter marked running after start");
    });

    runner.run("SimulatedAdapter start is idempotent", [&]() {
        EventDispatcher dispatcher;
        DeltaEngine deltaEngine;
        dispatcher.subscribe(&deltaEngine);

        SimulatedAdapter adapter(
            dispatcher,
            Exchange::BSE,
            {{"TCS", 4000.0, 50.0, Side::BUY}});

        adapter.start();
        adapter.start();

        EXPECT_EQUAL(runner, deltaEngine.getBuyVolume(), 50.0, "ticks not replayed twice");
    });
}
