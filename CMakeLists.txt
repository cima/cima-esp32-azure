cmake_minimum_required(VERSION 3.5)

set(AZURE_SDK_PATH_C "c:/Users/z608328/ESP32/esp-azure")
#SET(ARDUINO_BASE "c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1")
#set(ADAFRUIT_PATH "c:/Users/z608328/Documents/Arduino/libraries/Adafruit_BME280_Library")

set(CMAKE_MODULE_PATH $ENV{IDF_PATH})
set(CMAKE_SYSTEM_NAME Generic)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
#include($ENV{IDF_PATH}/tools/cmake/components.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
#set(CXX_FLAGS ${CXX_FLAGS} -std=c++17 -Wfatal-errors)

#add_compile_definitions(ARDUINO=150)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/config)

enable_language(CXX)

# ESP32 imports
# ------------- 

include_directories($ENV{IDF_PATH}/components/freertos/include)
include_directories($ENV{IDF_PATH}/components/esp32/include)
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
include_directories($ENV{IDF_PATH}/components/ethernet/include)

add_library(cxx IMPORTED STATIC GLOBAL)
add_library(soc IMPORTED STATIC GLOBAL)
add_library(heap IMPORTED STATIC GLOBAL)
add_library(driver IMPORTED STATIC GLOBAL)
add_library(log IMPORTED STATIC GLOBAL)
add_library(freertos IMPORTED STATIC GLOBAL)
add_library(esp_event IMPORTED STATIC GLOBAL)
add_library(tcpip_adapter IMPORTED STATIC GLOBAL)
add_library(lwip IMPORTED STATIC GLOBAL)
add_library(vfs IMPORTED STATIC GLOBAL)
add_library(ethernet IMPORTED STATIC GLOBAL)

# Azure imports
# -------------

set (AZURE_IOT_SDK "${AZURE_SDK_PATH_C}/azure-iot-sdk-c")
set (EXTRA_COMPONENT_DIRS ${EXTRA_COMPONENT_DIRS} $ENV{IDF_PATH}/components ${AZURE_SDK_PATH_C}/port)

# Adafruit imports
# ----------------

# TODO resolve ADAFRUIT sensor

# add_library(ada_bme280 STATIC ${ADAFRUIT_PATH}/Adafruit_BME280.cpp ${ADAFRUIT_PATH}/Adafruit_BME280.h ${ADAFRUIT_PATH}/Adafruit_sensor.h)
# include_directories(${ADAFRUIT_PATH})
# target_compile_features(ada_bme280 PUBLIC cxx_std_17)
 
# Main code
# ---------

project(CIMA-ESP32-AZURE)