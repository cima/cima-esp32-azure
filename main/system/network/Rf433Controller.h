#pragma once

#include <list>
#include <mutex>
#include <queue>
#include <chrono>
#include <functional>

#include <driver/gpio.h>

#include <boost/signals2/signal.hpp>

#include "../Log.h"

namespace cima::system::network {

    class Rf433Controller {
        
        static const Log LOGGER;

        const gpio_num_t rf433ReceiveGpioPin;
        std::list<std::function<void(void)>> receiveHandlers;

        unsigned long lastInterruptTime;
        unsigned long interruptCounter;
        
        std::queue<unsigned int> clicks;
        std::mutex clicksMutex;

        // TODO >>>
        
     

        boost::signals2::signal<void ()> buttonSignal;
        boost::signals2::signal<void ()> longButtonSignal;
        //<<< TODO

    public:
        Rf433Controller(gpio_num_t rf433ReceiveGpioPin);
        void initRf433();
        void handleData();

        void addReceiveHandler(std::function<void(void)> func);

    private:
        void rf433ReceiveHandler();
        static void rf433ReceiveHandlerWrapper(void *);
    };
}