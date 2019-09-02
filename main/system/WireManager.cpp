#include "WireManager.h"
#include "ManagerNotInitializedException.h"

namespace cima::system {

    bool WireManager::initialized = false;

    WireManager::WireManager(uint32_t setBits, uint32_t resetBits) 
            : setBits(setBits), resetBits(resetBits) {};

    void WireManager::init() {
        Wire.begin(setBits, resetBits);
        initialized = true;
    }

    TwoWire *WireManager::getWire() {
        if( ! initialized) {
            throw ManagerNotInitializedException();
        }

        return &Wire;
    }
}