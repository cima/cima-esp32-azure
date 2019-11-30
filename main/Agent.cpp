#include "Agent.h"

#include <thread>
#include <chrono>
#include <ctime>  
#include <string>
#include <cstring>

#include <stdio.h>

namespace cima {

    system::Log Agent::LOGGER("Agent");

    void Agent::welcome(std::string &visitorName){
        while(true){
            LOGGER.info(":-) Hello %s :-)", visitorName.c_str());
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    int Agent::justPrint(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize){
        LOGGER.info("Payload :\n%.*s", size, payload);

        const char* responseMessage = "\"Thanks for asking\"";
        *responseSize = strlen(responseMessage) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return 200;
    }

    int Agent::whatIsTheTime(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize){

        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        const char * timeText = std::ctime(&end_time);

        char responseMessage[512];
        *responseSize =  sprintf(responseMessage, "{\"time\":\"%s\"}", timeText) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return 200;
    }

    void Agent::mainLoop(){
        while(keepRunning){
            for(auto function : mainLoopFunctions){
                function();
            }

            //TODO safety slowdown to avoid starvation - to be removed once finalized whole concept
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void Agent::registerToMainLoop(std::function<void()> function){
        mainLoopFunctions.push_back(function);
    }

    void Agent::stop(){
        keepRunning = false;
    }
}