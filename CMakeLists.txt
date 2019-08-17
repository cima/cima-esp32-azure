cmake_minimum_required(VERSION 3.5)

set(AZURE_SDK_PATH_C "c:/Users/z608328/ESP32/esp-azure")
SET(ARDUINO_BASE "c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1")
set(ADAFRUIT_PATH "c:/Users/z608328/Documents/Arduino/libraries/Adafruit_BME280_Library")

set(CMAKE_MODULE_PATH $ENV{IDF_PATH})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
include($ENV{IDF_PATH}/tools/cmake/components.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(ARDUINO=150)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/config)

# ESP32 imports
# ------------- 

include_directories($ENV{IDF_PATH}/components/esp32/include)
include_directories($ENV{IDF_PATH}/components/freertos/include)
include_directories($ENV{IDF_PATH}/components/soc/esp32/include)
include_directories($ENV{IDF_PATH}/components/soc/include)
include_directories($ENV{IDF_PATH}/components/heap/include)
include_directories($ENV{IDF_PATH}/components/driver/include)
include_directories($ENV{IDF_PATH}/components/log/include)
include_directories($ENV{IDF_PATH}/components/esp_event/include)
include_directories($ENV{IDF_PATH}/components/tcpip_adapter/include)
include_directories($ENV{IDF_PATH}/components/lwip/include)
include_directories($ENV{IDF_PATH}/components/lwip/include/apps)
include_directories($ENV{IDF_PATH}/components/lwip/lwip/src/include)
include_directories($ENV{IDF_PATH}/components/lwip/port/esp32/include)
include_directories($ENV{IDF_PATH}/components/vfs/include)
#include_directories($ENV{IDF_PATH}/components/newlib/include)
include_directories($ENV{IDF_PATH}/components/newlib/platform_include)
include_directories($ENV{IDF_PATH}/components/ethernet/include)

add_library(soc IMPORTED STATIC GLOBAL)
add_library(heap IMPORTED STATIC GLOBAL)
add_library(driver IMPORTED STATIC GLOBAL)
add_library(log IMPORTED STATIC GLOBAL)
add_library(freertos IMPORTED STATIC GLOBAL)
add_library(esp_event IMPORTED STATIC GLOBAL)
add_library(tcpip_adapter IMPORTED STATIC GLOBAL)
add_library(lwip IMPORTED STATIC GLOBAL)
add_library(vfs IMPORTED STATIC GLOBAL)
add_library(newlib IMPORTED STATIC GLOBAL)
add_library(ethernet IMPORTED STATIC GLOBAL)

# Arduino imports
# ---------------

include_directories(${ARDUINO_BASE}/cores/esp32)
include_directories(${ARDUINO_BASE}/variants/ttgo-lora32-v1) #TODO: parametrize board model

## Wire
add_library(Wire STATIC ${ARDUINO_BASE}/libraries/Wire/src/Wire.cpp)
include_directories(${ARDUINO_BASE}/libraries/Wire/src)

## SPI
add_library(SPI STATIC)
target_sources(SPI PUBLIC ${ARDUINO_BASE}/libraries/SPI/src/SPI.cpp)
include_directories(${ARDUINO_BASE}/libraries/SPI/src)

## WiFi
file(GLOB wifi_sources ${ARDUINO_BASE}/libraries/WiFi/src/*.cpp)
add_library(WiFi STATIC ${wifi_sources})
include_directories(${ARDUINO_BASE}/libraries/WiFi/src)

# Azure imports
# -------------

set (AZURE_IOT_SDK "${AZURE_SDK_PATH_C}/azure-iot-sdk-c")
set (EXTRA_COMPONENT_DIRS ${AZURE_SDK_PATH_C}/port)

# Adafruit imports
# ----------------

add_library(ada_bme280 STATIC ${ADAFRUIT_PATH}/Adafruit_BME280.cpp)
include_directories(${ADAFRUIT_PATH})

# Main code
# ---------

project(CIMA-ESP32-AZURE)

set(COMPONENT_DIRS main)
