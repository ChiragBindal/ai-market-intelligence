#include "TestRunner.h"

void runDeltaEngineTests(TestRunner& runner);
void runEventDispatcherTests(TestRunner& runner);
void runFootPrintLevelTests(TestRunner& runner);
void runFootPrintEngineTests(TestRunner& runner);
void runExhaustionEngineTests(TestRunner& runner);
void runImbalanceEngineTests(TestRunner& runner);
void runAbsorptionEngineTests(TestRunner& runner);

int main() {
    TestRunner runner;

    runDeltaEngineTests(runner);
    runEventDispatcherTests(runner);
    runFootPrintLevelTests(runner);
    runFootPrintEngineTests(runner);
    runExhaustionEngineTests(runner);
    runImbalanceEngineTests(runner);
    runAbsorptionEngineTests(runner);

    return runner.runAll();
}
