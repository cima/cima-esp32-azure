cmake_minimum_required(VERSION 3.5)

set(AZURE_SDK_PATH_C "c:/Users/z608328/ESP32/esp-azure")
#SET(ARDUINO_BASE "c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1")
#set(ADAFRUIT_PATH "c:/Users/z608328/Documents/Arduino/libraries/Adafruit_BME280_Library")

set(CMAKE_SYSTEM_NAME Generic)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_FLAGS -mlongcalls ${CMAKE_CXX_FLAGS})

find_package(Boost)

# ESP32 imports
# ------------- 


# Azure imports
# -------------

set (AZURE_IOT_SDK "${AZURE_SDK_PATH_C}/azure-iot-sdk-c")
set (EXTRA_COMPONENT_DIRS ${AZURE_SDK_PATH_C}/port)

# Adafruit imports
# ----------------

# TODO resolve ADAFRUIT sensor

# add_library(ada_bme280 STATIC ${ADAFRUIT_PATH}/Adafruit_BME280.cpp ${ADAFRUIT_PATH}/Adafruit_BME280.h ${ADAFRUIT_PATH}/Adafruit_sensor.h)
# include_directories(${ADAFRUIT_PATH})
# target_compile_features(ada_bme280 PUBLIC cxx_std_17)
 
# Main code
# ---------

project(CIMA-ESP32-AZURE)