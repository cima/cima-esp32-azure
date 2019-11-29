#pragma once

#include <string>

#include "system/Log.h"

namespace cima {
    class Agent {
        static system::Log logger;

        public:
            void welcome(std::string &visitorName);
    };
}