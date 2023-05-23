#pragma once

#include <string>
#include <list>
#include <map>
#include <functional>

#include "system/Log.h"
#include "system/network/WifiManager.h"
#include "iot/AzureConfig.h"
#include "iot/IoTHubManager.h"
#include "system/EnvironmentSensorManager.h"

#include "LightGroupService.h"

namespace cima {
    class Agent {
        static const system::Log LOGGER;

        std::list<std::function<void()>> mainLoopFunctions;

        bool keepRunning = true;

        uint32_t lastRfEventTime;

        public:

            static std::string FLASH_FILESYSTEM_MOUNT_PATH;

            void welcome(std::string &visitorName);
            void cat(const std::string &filename);

            int justPrint(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);
            int sineLight(cima::LightGroupMap &lightGroups, const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);
            int whatIsTheTime(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);

            void handleRfButton(LightGroupService &lightService, int protocol, long command);

            void initFlashStorage();
            bool mountFlashFileSystem();
            void setupNetwork(system::network::WifiManager &wifiManager);
            std::list<system::network::WifiCredentials> readWifiCredentials();
            iot::AzureConfig readAzureConfig();

            void mainLoop();
            void registerToMainLoop(std::function<void()> function);
            void stop();
    };
}