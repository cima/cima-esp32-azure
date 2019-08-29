#pragma once

#include <stdint.h>

namespace cima::system {
    class WireManager {

        uint32_t setBits;
        uint32_t resetBits;

        static bool initialized;

    public:
        WireManager(uint32_t setBits, uint32_t resetBits);

        void init();

        Wire *getWire();

    };
}