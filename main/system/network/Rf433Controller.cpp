#include "Rf433Controller.h"

#include <iostream>
#include <sstream>

#include <esp_timer.h>

namespace cima::system::network {
    
    const Log Rf433Controller::LOGGER("Rf433Controller");

    Rf433Controller::Rf433Controller(gpio_num_t rf433ReceiveGpioPin) 
        : rf433ReceiveGpioPin(rf433ReceiveGpioPin), 
          lastInterruptTime(0), interruptCounter(0) {}

    void Rf433Controller::initRf433(){
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_ANYEDGE;
        io_conf.pin_bit_mask = 1ULL << rf433ReceiveGpioPin;    
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

        esp_err_t gpioError = gpio_config(&io_conf);
        LOGGER.debug("RF 433 MHz pin result: 0x%x", gpioError);
        
        //TODO ISR service must be extracted from button handler as it is one for all
        esp_err_t handlerError = gpio_isr_handler_add(rf433ReceiveGpioPin, &Rf433Controller::rf433ReceiveHandlerWrapper, this);
        LOGGER.debug("RF 433 MHz handler installation status: 0x%x", handlerError);
        

        LOGGER.debug("RF 433 MHz controller overall fine");
    }

    void Rf433Controller::handleData(){
        
        
        std::queue<unsigned int> newClicks;

        {
            std::lock_guard<std::mutex> guard(clicksMutex);
            clicks.swap(newClicks);
        }

        if(newClicks.empty()){
            return;
        }

        LOGGER.info("D: -----------------------");
        while( ! newClicks.empty()){
            std::ostringstream delays;
            for(int i = 0; i < 100 && ! newClicks.empty(); i++){
                auto duration = newClicks.front();
                delays << " " << duration;
                newClicks.pop();
            }
            LOGGER.info("D: %s", delays.str().c_str());
        }
        

        //LOGGER.info("Handling button event from: %s", std::ctime(&eventTime));
        //buttonSignal();
        //clicks.pop();
    }

    void Rf433Controller::addReceiveHandler(std::function<void(void)> func){
        buttonSignal.connect(func);
    }

    void Rf433Controller::rf433ReceiveHandler(){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.

        const long time = esp_timer_get_time();
        const unsigned int duration = time - lastInterruptTime;

        lastInterruptTime = time;

        interruptCounter++;
        if(interruptCounter % 1000 == 0) {
            std::lock_guard<std::mutex> guard(clicksMutex);
            clicks.push(duration);
        }

        //std::lock_guard<std::mutex> guard(clicksMutex);
        //clicks.push(duration);
    }

    void Rf433Controller::rf433ReceiveHandlerWrapper(void *arg){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.
        ((Rf433Controller *)arg)->rf433ReceiveHandler();
    }
}