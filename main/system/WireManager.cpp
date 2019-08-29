#include "WireManager.h"

#include <Wire.h>

namespace cima::system {

    bool WireManager::initialized = false;

    WireManager::WireManager(uint32_t setBits, uint32_t resetBits) 
            : setBits(setBits), resetBits(resetBits) {};

    WireManager::init() {
        Wire.begin(this.setBits, this.resetBits);
        initialized = true;
    }

    Wire *WireManager::getWire() {
        if( ! initialized) {
            throw ManagerNotInitializedException();
        }

        return &Wire;
    }
}