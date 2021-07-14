#pragma once

#include <list>
#include <mutex>
#include <queue>
#include <chrono>
#include <functional>

#include <driver/gpio.h>
#include <driver/ledc.h>

#include <boost/signals2/signal.hpp>

#include "Log.h"

namespace cima::system {
    class PWMDriver {
        const gpio_num_t pwmGpioPin;


    public:
        PWMDriver(gpio_num_t pwmGpioPin);

        void update(uint32_t dutyCycle);
    };
}