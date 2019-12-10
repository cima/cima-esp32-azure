#include "GreetingService.h"

#include <thread>
#include <chrono>
#include <ctime>  
#include <string>
#include <cstring>
#include <utility>
#include <memory>

#include <driver/gpio.h>

namespace cima {

    const system::Log GreetingService::LOGGER("GreetingService");

    std::string GreetingService::MESSAGE_TEMPLATE = "{\
        \"greetings\":\"Hello %s!\", \
        \"Temperature\":%f, \
        \"Humidity\":%f, \
        \"PressurehPa\": %f, \
        \"Timestamp\":\"%s\"}";

    const gpio_num_t GreetingService::LED_GPIO_PIN = GPIO_NUM_2;

    GreetingService::GreetingService(iot::IoTHubManager &iotHubManager, system::EnvironmentSensorManager &environmentSensorManager) 
        : iotHubManager(iotHubManager), environmentSensorManager(environmentSensorManager){
            initLed();
    }

    //TODO tohle bude muset bejt samostatná služba, 
    //     protože GPIO se asi musí nastavit celý najednou
    void GreetingService::initLed(){
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT;
        io_conf.pin_bit_mask = 1ULL << LED_GPIO_PIN;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

        gpio_config(&io_conf);
    }

    void GreetingService::welcomeLoop(std::string &visitorName){
        while(keepRunning){
            welcome(visitorName);
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }
    
    void GreetingService::welcome(std::string &visitorName){
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

            // Blink the LED
            gpio_set_level(LED_GPIO_PIN, 1);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            gpio_set_level(LED_GPIO_PIN, 0);
            
    }

    void GreetingService::stop(){
        keepRunning = false;
    }

}