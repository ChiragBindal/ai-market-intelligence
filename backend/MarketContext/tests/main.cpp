#include "TestRunner.h"

void runInitialBalanceEngineTests(TestRunner& runner);

int main() {
    TestRunner runner;
    runInitialBalanceEngineTests(runner);
    return runner.runAll();
}
