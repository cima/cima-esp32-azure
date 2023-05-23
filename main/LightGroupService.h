#pragma once

#include "system/PWMDriver.h"
#include "LightSettings.h"

namespace cima {
    class LightGroupService {

        static cima::system::Log LOGGER;

        system::PWMDriver ledDriver;
        LightSettings &lightSettings;

        bool ready = false;

    public:
        LightGroupService(system::PWMDriver &ledDriver, LightSettings &lightSettings);
        
        void loop();
        void setReady(bool isReady);

        void demoLoop();

        LightSettings &getLightSettings();

        void toggleContolMode();
        void increaseDuty();
        void decreaseDuty();
    };

    typedef std::map<std::string, boost::reference_wrapper<cima::LightGroupService>> LightGroupMap;
}