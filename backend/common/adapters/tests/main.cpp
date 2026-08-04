#include "TestRunner.h"

void runSimulatedAdapterTests(TestRunner& runner);
void runBinanceAdapterTests(TestRunner& runner);

int main() {
    TestRunner runner;
    runSimulatedAdapterTests(runner);
    runBinanceAdapterTests(runner);
    return runner.runAll();
}