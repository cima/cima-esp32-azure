#include "WireManager.h"
#include "ManagerNotInitializedException.h"

namespace cima::system {

    WireManager::WireManager(gpio_num_t sclPin, gpio_num_t sdaPin)
        : sclPin(sclPin), sdaPin(sdaPin), 
            i2cBus(I2C_NUM_0, sclPin, sdaPin) {}

    WireManager::WireManager(i2c_port_t port, gpio_num_t sclPin, gpio_num_t sdaPin)
        : sclPin(sclPin), sdaPin(sdaPin), 
            i2cBus(port, sclPin, sdaPin) {}

    CI2CBus &WireManager::getWire() {
        return i2cBus;
    }
}