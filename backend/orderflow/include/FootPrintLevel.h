#pragma once

#include "Trade.h"

class FootPrintLevel {
private:
    double buyVolume;
    double sellVolume;
    double delta;

public:
    FootPrintLevel();
    void processTrade(const Trade& trade);
    double getBuyVolume() const;
    double getSellVolume() const;
    double getDelta() const;
};
