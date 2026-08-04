#include "LiveExchangeAdapters.h"

#include <stdexcept>

namespace {

[[noreturn]] void notImplemented(const char* venue) {
    throw std::runtime_error(std::string(venue) + " adapter not implemented yet");
}

}  // namespace

void NseAdapter::start() {
    notImplemented("NSE");
}

void NseAdapter::stop() {}

void BseAdapter::start() {
    notImplemented("BSE");
}

void BseAdapter::stop() {}
