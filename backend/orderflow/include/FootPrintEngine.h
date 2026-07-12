#pragma once

#include "FootPrintLevel.h"
#include "Trade.h"
#include <map>

class FootPrintEngine {
private:
    std::map<double, FootPrintLevel> footPrintLevels;

public:
    FootPrintEngine();
    void processTrade(const Trade& trade);
    const FootPrintLevel* getLevel(double price) const;
    const FootPrintLevel* getPrevLevel(double price) const;
    const FootPrintLevel* getNextLevel(double price) const;
    void reset();
};
