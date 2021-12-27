#pragma once

#include <map>
#include <functional>

#include <driver/gpio.h>

#include "system/Log.h"
#include "system/Log.h"

namespace cima {

    typedef std::map<unsigned int, unsigned int> ScheduleMap;

    class LightSettings {
        static system::Log LOGGER;
        static const std::string NO_FILE;

        ScheduleMap schedule;

        std::reference_wrapper<const std::string> jsonFile = std::ref(NO_FILE);

        gpio_num_t gpio_pin = GPIO_NUM_NC;

    public:
        /** Creates empty settings */
        LightSettings() {};

        /** Creates light settings refering to GPIO pin setting denoted by GPIO num */
        LightSettings(gpio_num_t gpio_pin);

        /**
         * Update lighting schedule based on given file containg JSON string representation of the schedule
         * example:
         *      {
         *          "8:00:00":0,
         *          "9:00:00":4000,
         *          "11:00:00":8000
         *      }
         * 
         * Return: True if update was successfull, false otherwise
        */
        bool updateFromFile(const std::string &jsonFile);

        /**
         * Update the time schedule from JSON string representation of the schedule
         * example:
         *      {
         *          "8:00:00":0,
         *          "9:00:00":4000,
         *          "11:00:00":8000
         *      }
         * 
         * Return: True if update was successfull, false otherwise
        */
        bool updateFromJson(const char *jsonString, bool isVolatile = false);

        /** Returns interpolated value from pallete with millisecond precission. */
        int getValueForMilis(int milliseconds);

    private:
        int getSecondsOfDayFromString(const char *strTime);
        bool storeScheduleToFile();
        
    };
}