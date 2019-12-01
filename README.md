# HW info

## Board info
[TTGO LoRa32 OLED](https://hackaday.io/project/27791-esp32-lora-oled-module)
[Manufacturer data sheet](http://www.lilygo.cn/prod_view.aspx?TypeId=50032&Id=1152)

## Sesnsor info
https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf

# Development info

## Azure IoT Hub
- https://github.com/Azure/azure-iot-sdk-c/blob/350b51f5abaedc975dae5419ad1fa4add7635fd2/iothub_client/samples/iothub_ll_client_x509_sample/iothub_ll_client_x509_sample.c

# Environment setup
- [Visual studio code](https://code.visualstudio.com/)
  - https://github.com/Deous/VSC-Guide-for-esp32
- git

## Dependencies


### ESP 4.0 (beta 2)
- [Getting started ESP 4.0 (beta 2)](https://docs.espressif.com/projects/esp-idf/en/v4.0-beta2/get-started/index.html)
- [ESP-IDF](https://github.com/espressif/esp-idf)
```
    git clone -b release/v4.0 --recursive https://github.com/espressif/esp-idf.git
```
- [ESP-toolchain 4.0 beta 2 Installer](https://docs.espressif.com/projects/esp-idf/en/v4.0-beta2/get-started/windows-setup.html)
  - python 3.6+
  - cmake 3.13
  - mingw32-make 3.82

### Azure IoT SDK C for ESP
- [ESP Azure IoT SDK](https://github.com/espressif/esp-azure)
```
    git clone --recursive https://github.com/espressif/esp-azure.git
    git submodule update --init --recursive
```
### BOOST
- [Getting started with boost](https://www.boost.org/doc/libs/1_71_0/more/getting_started/windows.html)
- Ensure environment variable *BOOST_ROOT* that contain path to unpacked boost distribution.

### Misc

- Adafruit drivers for Arduino
  - Patch adafruit driver
- [Disable FreeRTOS static allocation API](https://esp32.com/viewtopic.php?t=3504) set in [sdkconfig.defaults](sdkconfig.defaults)
- [Quartz & Bitrate settings](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started-connect)

### Adafruit drivers

### Wire

# Deprecated

## Arduino like
- clone with submodules https://github.com/espressif/esp-azure to c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1/libraries/esp-azure/