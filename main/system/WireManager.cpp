#include "WireManager.h"
#include "ManagerNotInitializedException.h"

namespace cima::system {

    bool WireManager::initialized = false;

    WireManager::WireManager(uint32_t setBits, uint32_t resetBits) 
            : setBits(setBits), resetBits(resetBits) {};

    void WireManager::init() {
        //TODO Wire.begin(setBits, resetBits);
        initialized = true;
    }

    void *WireManager::getWire() {
        if( ! initialized) {
            throw ManagerNotInitializedException();
        }

        //TODO
        return nullptr;
    }
}