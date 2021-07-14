#pragma once

#include "system/PWMDriver.h"

namespace cima {
    class LightAlarmService {

        system::PWMDriver ledDriver;
        bool keepRunning = true;

    public:
        LightAlarmService(system::PWMDriver &ledDriver);
        void loop();
    };
}