#include "TestRunner.h"

void runDeltaEngineTests(TestRunner& runner);
void runFootPrintLevelTests(TestRunner& runner);
void runFootPrintEngineTests(TestRunner& runner);

int main() {
    TestRunner runner;

    runDeltaEngineTests(runner);
    runFootPrintLevelTests(runner);
    runFootPrintEngineTests(runner);

    return runner.runAll();
}
