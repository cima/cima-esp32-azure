# HW info

## Board info
TTGO LoRa32 OLED

## Sesnsor info
https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf

# Development info

## Azure IoT Hub
- https://github.com/Azure/azure-iot-sdk-c/blob/350b51f5abaedc975dae5419ad1fa4add7635fd2/iothub_client/samples/iothub_ll_client_x509_sample/iothub_ll_client_x509_sample.c

# Environment setup
- (Visual studio code)[https://code.visualstudio.com/]
- https://github.com/Deous/VSC-Guide-for-esp32
- git
- python 3.6
- cmake 3.15
- mingw32-make 3.82

## Dependencies

- ESP-IDF 3.3
- Azure IoT SDK C
- Adafruit drivers for Arduino
  - Patch adafruit driver
- Arduino

### ESP-IFD
- Tools: https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html
- dependencies

    git clone --recursive https://github.com/espressif/esp-idf.git

prefferably clone some up to date release versioin >= 3.3 (see more at https://github.com/espressif/esp-idf/)

### Azure IoT SDK
- clone with submodules 

    git clone --recursive https://github.com/espressif/esp-azure

### Adafruit drivers

### Wire

# Deprecated

## Arduino like
- clone with submodules https://github.com/espressif/esp-azure to c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1/libraries/esp-azure/