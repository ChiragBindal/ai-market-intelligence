#include "ExhaustionEngine.h"
#include "TestRunner.h"

#include <deque>
#include <string>

namespace {

void feedBuyVolumes(ExhaustionEngine& engine, const std::deque<double>& volumes) {
    for (double volume : volumes) {
        engine.processObservation(Side::BUY, volume);
    }
}

void feedSellVolumes(ExhaustionEngine& engine, const std::deque<double>& volumes) {
    for (double volume : volumes) {
        engine.processObservation(Side::SELL, volume);
    }
}

bool dequeEquals(const std::deque<double>& actual, const std::deque<double>& expected) {
    if (actual.size() != expected.size()) {
        return false;
    }
    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            return false;
        }
    }
    return true;
}

}  // namespace

void runExhaustionEngineTests(TestRunner& runner) {
    runner.run("ExhaustionEngine - Test Case 1: Buy Exhaustion", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {5000, 4200, 3400, 2500, 1800});

        EXPECT_TRUE(runner, engine.isBuyExhaustion(), "isBuyExhaustion() == true");
        EXPECT_TRUE(runner, !engine.isSellExhaustion(), "sell side not exhausted");
    });

    runner.run("ExhaustionEngine - Test Case 2: No Exhaustion", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {5000, 5100, 4900, 5200, 5000});

        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "fluctuating volumes are not exhaustion");
    });

    runner.run("ExhaustionEngine - Test Case 3: Increasing Buying", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {1000, 2000, 3000, 4000, 5000});

        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "increasing buy volume is not exhaustion");
    });

    runner.run("ExhaustionEngine - Test Case 4: Sell Exhaustion", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedSellVolumes(engine, {6000, 5000, 4200, 3000, 1800});

        EXPECT_TRUE(runner, engine.isSellExhaustion(), "isSellExhaustion() == true");
        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "buy side not exhausted");
    });

    runner.run("ExhaustionEngine - Test Case 5: Rolling Window", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {6000, 5200, 4300, 3500, 2800});
        engine.processObservation(Side::BUY, 2000);

        const std::deque<double> expected = {5200, 4300, 3500, 2800, 2000};
        EXPECT_TRUE(runner, dequeEquals(engine.getBuyVolumes(), expected), "rolling window drops oldest value");
    });

    runner.run("ExhaustionEngine - Test Case 6: Not Enough Data", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {5000, 4200, 3000});

        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "insufficient observations return false");
        EXPECT_TRUE(runner, engine.getBuyVolumes().size() == 3, "stores partial window");
    });

    runner.run("ExhaustionEngine - Test Case 7: Small Fluctuations Ignored", [&]() {
        ExhaustionEngine engine(5, 0.10, 3);

        feedBuyVolumes(engine, {5000, 4900, 4800, 4700, 4600});

        EXPECT_TRUE(runner, !engine.isBuyExhaustion(), "~2% declines below 10% threshold");
    });
}
