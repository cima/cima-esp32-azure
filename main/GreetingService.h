#pragma once

#include "iot/IoTHubManager.h"
#include "system/EnvironmentSensorManager.h"

namespace cima {
    class GreetingService {

        static const system::Log LOGGER;
        static std::string MESSAGE_TEMPLATE;

        iot::IoTHubManager &iotHubManager;
        system::EnvironmentSensorManager &environmentSensorManager;

        bool keepRunning = true;

    public:
        GreetingService(iot::IoTHubManager &iotHubManager, system::EnvironmentSensorManager &environmentSensorManager);
        void welcome(std::string &visitorName);

        void stop();
    };
}