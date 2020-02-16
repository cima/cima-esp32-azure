#pragma once

#include <list>
#include <mutex>
#include <queue>
#include <chrono>
#include <functional>

#include <driver/gpio.h>

#include <boost/signals2/signal.hpp>

#include "Log.h"

namespace cima::system {

    class ButtonController {
        
        static const Log LOGGER;

        const gpio_num_t buttonGpioPin;

        std::list<std::function<void(void)>> buttonHandlers;

        std::queue<std::time_t> clicks;
        std::mutex clicksMutex;

        boost::signals2::signal<void ()> buttonSignal;

        boost::signals2::signal<void ()> longButtonSignal;

    public:
        ButtonController(gpio_num_t buttonGpioPin);
        void initButton();
        void handleClicks();
        void addHandler(std::function<void(void)> func);
        void addLongPressHandler(std::function<void(void)> func);

    private:
        void gpioButtonHandler();
        static void gpioButtonHandlerWrapper(void *);
    };
}