#pragma once

#include <map>
#include <functional>

#include "system/Log.h"

namespace cima {

    typedef std::map<unsigned int, unsigned int> ScheduleMap;

    class LightSettings {
        static system::Log LOGGER;
        static const std::string NO_FILE;

        ScheduleMap schedule;

        std::reference_wrapper<const std::string> jsonFile = std::ref(NO_FILE);

    public:
        /** Creates empty settings */
        LightSettings() {};

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
        bool updateFromJson(const char *jsonString);

    private:
        int getSecondsOfDayFromString(const char *strTime);
        bool storeScheduleToFile();
        
    };
}