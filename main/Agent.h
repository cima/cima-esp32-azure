#pragma once

#include <string>
#include <list>
#include <functional>

#include "system/Log.h"

namespace cima {
    class Agent {
        static system::Log LOGGER;

        std::list<std::function<void()>> mainLoopFunctions;

        bool keepRunning = true;
        public:
            void welcome(std::string &visitorName);
            int justPrint(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);
            int whatIsTheTime(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);

            void mainLoop();
            void registerToMainLoop(std::function<void()> function);
            void stop();
    };
}