set(srcs
    "CimaEsp32Agent.cpp"
    "Agent.h" "Agent.cpp"
    "GreetingService.h" "GreetingService.cpp"
    "LightAlarmService.h" "LightAlarmService.cpp"
    "system/Log.h" "system/Log.cpp"
    "system/WireManager.h" "system/WireManager.cpp"
    "system/ButtonController.h" "system/ButtonController.cpp"
    "system/PWMDriver.h" "system/PWMDriver.cpp"
    "system/EnvironmentSensorManager.h" "system/EnvironmentSensorManager.cpp"
    "system/ManagerNotInitializedException.h"
    "system/ExecutionLimiter.h" "system/ExecutionLimiter.cpp"

    "system/network/NetworkManager.h" "system/network/NetworkManager.cpp"
    "system/network/WifiManager.h" "system/network/WifiManager.cpp"
    "system/network/WifiCredentials.h" "system/network/WifiCredentials.cpp"
    "system/network/ModemManager.h" "system/network/ModemManager.cpp"
    "system/network/M590.h" "system/network/M590.cpp"

    "iot/CertSource.cpp" "iot/CertSource.h"
    "iot/AzureConfig.cpp" "iot/AzureConfig.h"
    "iot/IotHubManager.cpp" "iot/IotHubManager.h"
    "iot/DeviceProvisioningClient.cpp" "iot/DeviceProvisioningClient.h"
    "display/Display.h" "display/Display.cpp"
    "display/StatusIcon.h" "display/StatusIcon.cpp"
    ${AZURE_IOT_SDK}/certs/certs.c
)

idf_component_register(SRCS "${srcs}"
                    REQUIRES bme280 ssd1306 spiffs nvs_flash json mbedtls port esp_eth driver lwip modem
) 

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

component_compile_definitions(CORE_DEBUG_LEVEL=4)

# Boost
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

# Azure SDK C specialities
include_directories(${AZURE_IOT_SDK}/certs)
add_definitions(-DSET_TRUSTED_CERT_IN_SAMPLES)
component_compile_definitions(SET_TRUSTED_CERT_IN_SAMPLES)

spiffs_create_partition_image(storage ../filesystem FLASH_IN_PROJECT)