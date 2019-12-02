#pragma once

// TODO resolve adafruit sensor
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

#include "WireManager.h"

#include <iot_bme280.h>

namespace cima::system {
    class EnvironmentSensorManager {

        WireManager &wireManager;
        
        CBme280 bme280;

    public:
        EnvironmentSensorManager(WireManager &wireManager);
        void init();

        float readTemperature();
        float readHumidity();
        float readPressure();
    };
}