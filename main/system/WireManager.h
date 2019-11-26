#pragma once

#include <stdint.h>
//#include <Wire.h>

namespace cima::system {
    class WireManager {

        uint32_t setBits;
        uint32_t resetBits;

        static bool initialized;

    public:
        WireManager(uint32_t setBits, uint32_t resetBits);

        void init();

        void *getWire();

    };
}