# include "EnvironmentSensorManager.h"

namespace cima::system {
    EnvironmentSensorManager::EnvironmentSensorManager(WireManager &wireManager)
        : wireManager(wireManager) {}

    void EnvironmentSensorManager::init() {
        //bme.begin(wireManager.getWire());  
    }

    float EnvironmentSensorManager::readTemperature() {
        //return (float)bme.readTemperature();
        return 0.0;
    }   
    
    float EnvironmentSensorManager::readHumidity() {
        //return bme.readHumidity();
        return 0.0;
    }
}