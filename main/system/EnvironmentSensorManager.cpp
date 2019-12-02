# include "EnvironmentSensorManager.h"

namespace cima::system {
    EnvironmentSensorManager::EnvironmentSensorManager(WireManager &wireManager)
        : wireManager(wireManager), bme280(&wireManager.getWire()) {}

    void EnvironmentSensorManager::init() {
        bme280.init();
    }

    float EnvironmentSensorManager::readTemperature() {
        return bme280.temperature();
    }   
    
    float EnvironmentSensorManager::readHumidity() {
        return bme280.humidity();
    }

    float EnvironmentSensorManager::readPressure() {
        return bme280.pressure();
    }
}