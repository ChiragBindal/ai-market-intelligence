#include "InitialBalanceEngine.h"
#include "TestHelpers.h"
#include "TestRunner.h"

#include <chrono>

using Duration = std::chrono::minutes;

void runInitialBalanceEngineTests(TestRunner& runner) {
    runner.run("InitialBalanceEngine - Test 1: IB High/Low from in-window trades", [&]() {
        const auto sessionStart = minutesFromEpoch(9 * 60 + 15);  // 09:15
        InitialBalanceEngine engine(sessionStart, Duration(60));

        engine.processTrade(makeTrade(Side::BUY, 1, 100.0, sessionStart + Duration(1)));
        engine.processTrade(makeTrade(Side::BUY, 1, 102.0, sessionStart + Duration(2)));
        engine.processTrade(makeTrade(Side::BUY, 1, 101.0, sessionStart + Duration(3)));
        engine.processTrade(makeTrade(Side::BUY, 1, 99.0, sessionStart + Duration(4)));

        EXPECT_EQUAL(runner, engine.getInitialBalanceHigh(), 102.0, "IB High");
        EXPECT_EQUAL(runner, engine.getInitialBalanceLow(), 99.0, "IB Low");
        EXPECT_TRUE(runner, !engine.isInitialBalanceFinalized(), "IB not finalized before window ends");
    });

    runner.run("InitialBalanceEngine - Test 2: trades after window do not change IB", [&]() {
        const auto sessionStart = minutesFromEpoch(9 * 60 + 15);
        InitialBalanceEngine engine(sessionStart, Duration(60));

        engine.processTrade(makeTrade(Side::BUY, 1, 100.0, sessionStart + Duration(10)));
        engine.processTrade(makeTrade(Side::BUY, 1, 102.0, sessionStart + Duration(20)));
        engine.processTrade(makeTrade(Side::BUY, 1, 99.0, sessionStart + Duration(30)));

        EXPECT_EQUAL(runner, engine.getInitialBalanceHigh(), 102.0, "IB High before window ends");
        EXPECT_EQUAL(runner, engine.getInitialBalanceLow(), 99.0, "IB Low before window ends");

        engine.processTrade(makeTrade(Side::BUY, 1, 150.0, sessionStart + Duration(61)));
        engine.processTrade(makeTrade(Side::SELL, 1, 80.0, sessionStart + Duration(90)));

        EXPECT_EQUAL(runner, engine.getInitialBalanceHigh(), 102.0, "IB High unchanged after window");
        EXPECT_EQUAL(runner, engine.getInitialBalanceLow(), 99.0, "IB Low unchanged after window");
        EXPECT_TRUE(runner, engine.isInitialBalanceFinalized(), "IB finalized after window ends");
    });

    runner.run("InitialBalanceEngine - Test 3: boundary trade excluded (>= window end)", [&]() {
        const auto sessionStart = minutesFromEpoch(9 * 60 + 15);  // 09:15
        const Duration window(60);
        const auto windowEnd = sessionStart + window;  // 10:15

        InitialBalanceEngine engine(sessionStart, window);

        engine.processTrade(makeTrade(Side::BUY, 1, 100.0, sessionStart + Duration(10)));
        engine.processTrade(makeTrade(Side::BUY, 1, 102.0, windowEnd - Duration(1)));

        EXPECT_EQUAL(runner, engine.getInitialBalanceHigh(), 102.0, "IB High before boundary");
        EXPECT_EQUAL(runner, engine.getInitialBalanceLow(), 100.0, "IB Low before boundary");

        engine.processTrade(makeTrade(Side::BUY, 1, 200.0, windowEnd));

        EXPECT_EQUAL(runner, engine.getInitialBalanceHigh(), 102.0, "boundary trade excluded from IB High");
        EXPECT_EQUAL(runner, engine.getInitialBalanceLow(), 100.0, "boundary trade excluded from IB Low");
        EXPECT_TRUE(runner, engine.isInitialBalanceFinalized(), "IB finalized at boundary trade");
    });
}
