#include "Agent.h"

#include <thread>
#include <chrono>

namespace cima {

    system::Log Agent::logger("Agent");

    void Agent::welcome(std::string &visitorName){
        while(true){
            logger.info(":-) Hello %s :-)", visitorName.c_str());
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
}