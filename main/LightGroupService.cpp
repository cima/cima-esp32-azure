#include "LightGroupService.h"

#include <thread>
#include <chrono>

#include <cmath>

namespace cima {

    ::cima::system::Log LightGroupService::LOGGER("LightAlarmService");

    LightGroupService::LightGroupService(system::PWMDriver &ledDriver, LightSettings &lightSettings) : ledDriver(ledDriver), lightSettings(lightSettings) {}

    void LightGroupService::loop(){

        if ( ! ready) {
            //LOGGER.info("Not ready");
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        auto timer = std::chrono::system_clock::to_time_t(now);
        std::tm bt = *std::localtime(&timer);
        int seconds = bt.tm_hour*60*60 + bt.tm_min*60 + bt.tm_sec;

        auto milis = seconds * 1000 + (int)ms.count();
        
        //LOGGER.info("loop milis: %d", milis);

        auto duty = lightSettings.getValueForMilis(milis);
        //LOGGER.info("loop duty: %d @ %d", duty, milis);

        ledDriver.update((uint32_t)duty);

    }

    void LightGroupService::setReady(bool isReady){
        ready = isReady;
    }

    void LightGroupService::demoLoop(){
        bool currentReady = this->ready;
        setReady(false);

        //approx. 5 seconds of sine wave intensity
        for(int i = 0; i < 500; i++){
            
            struct timespec timeinfo;
            clock_gettime(CLOCK_MONOTONIC, &timeinfo);
            uint32_t milis = (timeinfo.tv_sec*1000 + timeinfo.tv_nsec/1000000);
            uint32_t duty = (uint32_t)((sin(3.14159265 / 2000 * milis) + 1) * 4096);

            ledDriver.update(duty);

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        setReady(currentReady);
    }

    LightSettings &LightGroupService::getLightSettings() {
        return this->lightSettings;
    }
}