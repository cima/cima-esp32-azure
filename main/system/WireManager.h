#pragma once

#include <stdint.h>
#include <driver/i2c.h>
#include <iot_i2c_bus.h>

namespace cima::system {
    class WireManager {

        // Clock pin
        gpio_num_t sclPin; 

        // Data pin
        gpio_num_t sdaPin;

        // Configured wire
        CI2CBus i2cBus;

    public:
        WireManager(gpio_num_t sclPin, gpio_num_t sdaPin);
        WireManager(i2c_port_t port, gpio_num_t sclPin, gpio_num_t sdaPin);
        CI2CBus &getWire();
    };
}