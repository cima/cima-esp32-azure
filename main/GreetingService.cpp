#include "GreetingService.h"

#include <thread>
#include <chrono>
#include <ctime>  
#include <string>
#include <cstring>
#include <utility>
#include <memory>

namespace cima {

    const system::Log GreetingService::LOGGER("GreetingService");

    std::string GreetingService::MESSAGE_TEMPLATE = "{\
        \"greetings\":\"Hello %s!\", \
        \"Temperature\":%f, \
        \"Humidity\":%f, \
        \"PressurehPa\": %f, \
        \"Timestamp\":\"%s\"}";

    GreetingService::GreetingService(iot::IoTHubManager &iotHubManager, system::EnvironmentSensorManager &environmentSensorManager) 
        : iotHubManager(iotHubManager), environmentSensorManager(environmentSensorManager){}

    void GreetingService::welcome(std::string &visitorName){
        while(keepRunning){

            auto end = std::chrono::system_clock::now();
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);

            char greeting[256];
            sprintf(greeting, MESSAGE_TEMPLATE.c_str(), 
                visitorName.c_str(), 
                environmentSensorManager.readTemperature(), 
                environmentSensorManager.readHumidity(),
                environmentSensorManager.readPressure(),
                std::ctime(&end_time)
            );

            LOGGER.info(":-) Hello %s :-) %f °C, %f %%, %f hPa", 
                visitorName.c_str(),
                environmentSensorManager.readTemperature(), 
                environmentSensorManager.readHumidity(),
                environmentSensorManager.readPressure()
            );
            if(iotHubManager.isReady()) {
                iotHubManager.sendMessage(greeting);
            }

            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }

    void GreetingService::stop(){
        keepRunning = false;
    }
}