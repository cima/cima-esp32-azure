cmake_minimum_required(VERSION 3.5)

set(AZURE_SDK_C_PATH $ENV{AZURE_SDK_C_PATH})

set(CMAKE_SYSTEM_NAME Generic)

# ESP32 imports
# ------------- 
message(INFO " IoT solution is at: $ENV{IOT_SOLUTION_PATH}")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_FLAGS -mlongcalls ${CMAKE_C_FLAGS})
set(CMAKE_CXX_FLAGS -mlongcalls ${CMAKE_CXX_FLAGS})

cmake_policy(SET CMP0074 NEW)
find_package(Boost)

# Azure imports
# -------------
set (AZURE_IOT_SDK "${AZURE_SDK_C_PATH}/azure-iot-sdk-c")
set (EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${AZURE_SDK_C_PATH}/port")

# Adafruit sensor
# ---------------
set(IOT_COMPONENT_DIRS  ${IOT_COMPONENT_DIRS}
                        $ENV{IOT_SOLUTION_PATH}/components/i2c_devices
                        $ENV{IOT_SOLUTION_PATH}/components/i2c_devices/sensor
                        $ENV{IOT_SOLUTION_PATH}/components/i2c_devices/others )

set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${IOT_COMPONENT_DIRS}")

# Modem & PPPoS implementation
# ----------------------------
set(EXTRA_COMPONENT_DIRS "${EXTRA_COMPONENT_DIRS} ${IDF_PATH}/examples/protocols/pppos_client/components")

# Main code
# ---------

project(CIMA-ESP32-AZURE)