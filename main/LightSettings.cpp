#include "LightSettings.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>
#include <iomanip>
#include <time.h>
#include <math.h>
#include <ctime>

#include <cJSON.h>

namespace cima {
    ::cima::system::Log LightSettings::LOGGER("LightSettings");
    const std::string LightSettings::NO_FILE = std::string();

    LightSettings::LightSettings(gpio_num_t gpio_pin) : gpio_pin(gpio_pin) {};

    bool LightSettings::updateFromFile(const std::string &jsonFile) {
        this->jsonFile = std::reference_wrapper(jsonFile);

        std::ifstream in(jsonFile, std::ios_base::binary);

        if( ! in.is_open()) {
            LOGGER.info("No such file : %s", jsonFile.c_str());
            return false;
        }

        LOGGER.info("Updating form File: %s", jsonFile.c_str());

        std::string jsonString((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));

        if(in.fail()) {
            return false;
        }

        return updateFromJson(jsonString.c_str(), true);
    }

    bool LightSettings::updateFromJson(const char *jsonString, bool isVolatile){
        if ( ! jsonString) {
            LOGGER.error("Null jsonString provided.");
            return false;
        }

        LOGGER.debug("Obtained JSON schedule: %s", jsonString);
        cJSON *root = cJSON_Parse(jsonString);
        if ( ! root) {
            LOGGER.error("Couldn't parse jsonString into JSON.");
            return false;
        }

        LOGGER.info("Updating form JSON: %s", jsonString);

        ScheduleMap newSchedule;          
        bool errors = false;

        cJSON *item = root->child;
        while (item) {
            
            const char *timeStr = item->string;
            int seconds = getSecondsOfDayFromString(timeStr);
            if (seconds == -1) {
                LOGGER.info("Invalid time scpecification: %s", timeStr);
                errors = true;
                continue;
            }

            int level = item->valueint;

            newSchedule[seconds] = level;

            LOGGER.info("Adding time %s(%d s) with value %d", timeStr, seconds, level);
            item = item->next;
        }

        cJSON_free(root);

        if (errors) {
            return false;
        }

        schedule.swap(newSchedule);

        return isVolatile || storeScheduleToFile();
    }

    bool LightSettings::storeScheduleToFile() {
        LOGGER.info("JsonFile: '%s', NO_FILE: '%s'", jsonFile.get().c_str(), NO_FILE.c_str());
        if(NO_FILE.compare(jsonFile.get()) == 0) {
            LOGGER.info("No file specified for light settings JSON storage.");
            return true;
        }

        LOGGER.info("File '%s' specified for light settings", jsonFile.get().c_str());
        std::ofstream out(jsonFile, std::ios_base::binary);
        bool comma = false;

        out << "{" << std::endl;
        std::for_each(schedule.begin(), schedule.end(), [&](std::pair<int, int> schedulePair) {
            std::tm *timeStruct = std::localtime((const time_t *)&schedulePair.first);
            if(timeStruct){
                if(comma) {
                    out << "," << std::endl;
                }
                out << "\"" << std::put_time(timeStruct, "%H:%M:%S") << "\": " << schedulePair.second;
                comma = true;
            }

        });
        out << std::endl << "}" << std::endl;

        return ! out.fail();
    }

    int LightSettings::getValueForMilis(int milliseconds){
        double requestedSconds = milliseconds / 1000.0;
        std::pair<int, int> startPair = *schedule.rbegin();
        std::for_each(schedule.begin(), schedule.end(), [&](std::pair<int, int> schedulePair) {
            if(schedulePair.first <= requestedSconds){
                startPair = schedulePair;
            }
        });

        std::pair<int, int> endPair = *schedule.begin();
        std::for_each(schedule.rbegin(), schedule.rend(), [&](std::pair<int, int> schedulePair) {
            if(schedulePair.first >= requestedSconds){
                endPair = schedulePair;
            }
        });

        double t_diff = (endPair.first - startPair.first)*1000;
        double param = (milliseconds - startPair.first*1000)/t_diff;
        double v_diff = endPair.second - startPair.second;

        return (int)floor(startPair.second + param * v_diff); 
    }

    int LightSettings::getSecondsOfDayFromString(const char *strTime) {
        LOGGER.info("Converting %s to seconds of the day", strTime);

        std::tm timeStruct;
        if(strptime(strTime, "%H:%M:%S", &timeStruct)) {
            return timeStruct.tm_hour * 3600 + timeStruct.tm_min * 60 + timeStruct.tm_sec;
        } else {
            return -1;
        }
    }
}