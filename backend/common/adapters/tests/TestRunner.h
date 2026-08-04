#pragma once

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class TestRunner {
public:
    void run(const std::string& suiteName, const std::function<void()>& fn) {
        ++suitesRun_;
        currentSuite_ = suiteName;
        std::cout << "[" << suiteName << "]" << std::endl;
        fn();
    }

    void check(bool condition, const std::string& name) {
        ++testsRun_;
        if (condition) {
            ++testsPassed_;
            std::cout << "  PASS " << name << std::endl;
            return;
        }
        ++testsFailed_;
        std::cout << "  FAIL " << name;
        if (!currentSuite_.empty()) {
            std::cout << " (" << currentSuite_ << ")";
        }
        std::cout << std::endl;
    }

    void checkEqual(double actual, double expected, const std::string& name) {
        check(std::abs(actual - expected) < 1e-9, name + " (expected " + std::to_string(expected)
            + ", got " + std::to_string(actual) + ")");
    }

    int runAll() const {
        std::cout << std::endl;
        std::cout << testsPassed_ << "/" << testsRun_ << " tests passed";
        if (testsFailed_ > 0) {
            std::cout << ", " << testsFailed_ << " failed";
        }
        std::cout << std::endl;
        return testsFailed_ == 0 ? 0 : 1;
    }

private:
    int suitesRun_ = 0;
    int testsRun_ = 0;
    int testsPassed_ = 0;
    int testsFailed_ = 0;
    std::string currentSuite_;
};

#define EXPECT_TRUE(runner, expr, name) (runner).check((expr), name)
#define EXPECT_EQUAL(runner, actual, expected, name) (runner).checkEqual((actual), (expected), name)
