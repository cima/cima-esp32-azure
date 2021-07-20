#include "LightAlarmService.h"

#include <thread>
#include <chrono>

#include <cmath>

namespace cima {

    ::cima::system::Log LightAlarmService::LOGGER("LightAlarmService");

    LightAlarmService::LightAlarmService(system::PWMDriver &ledDriver) : ledDriver(ledDriver) {}

    void LightAlarmService::loop(){
        while(keepRunning){
            
            struct timespec timeinfo;
            clock_gettime(CLOCK_MONOTONIC, &timeinfo);
            uint32_t milis = (timeinfo.tv_sec*1000 + timeinfo.tv_nsec/1000000);
            uint32_t duty = (uint32_t)((sin(3.14159265 / 2000 * milis) + 1) * 4096);

            ledDriver.update(duty);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}