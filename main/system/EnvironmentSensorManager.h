#pragma once

// TODO resolve adafruit sensor
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

#include "WireManager.h"

namespace cima::system {
    class EnvironmentSensorManager {

        WireManager &wireManager;
        //Adafruit_BME280 bme; // I2C

    public:
        EnvironmentSensorManager(WireManager &wireManager);
        void init();

        float readTemperature();
        float readHumidity();
    };
}