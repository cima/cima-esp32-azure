#pragma once

#include <string>
#include <list>
#include <functional>

#include "system/Log.h"
#include "system/WifiManager.h"

namespace cima {
    class Agent {
        static system::Log LOGGER;

        static std::string FLASH_FILESYSTEM_MOUNT_PATH;

        std::list<std::function<void()>> mainLoopFunctions;

        bool keepRunning = true;
        public:
            void welcome(std::string &visitorName);
            void cat(const std::string &filename);

            int justPrint(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);
            int whatIsTheTime(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);

            bool mountFlashFileSystem();
            void setupNetwork(system::WifiManager &wifiManager);
            std::pair<std::string, std::string> readWifiCredentials();

            void mainLoop();
            void registerToMainLoop(std::function<void()> function);
            void stop();
    };
}