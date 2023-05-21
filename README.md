# HW info

## Board info
- [LILYGO® 4M Byte (32M bit )Pro ESP32 OLED V2.0 TTGO](http://www.lilygo.cn/prod_view.aspx?TypeId=50032&Id=1152) Manufacturer data sheet
- [TTGO LoRa32 OLED](https://hackaday.io/project/27791-esp32-lora-oled-module) usefull fan page.

![](./Lilygo-TTGO-wiring.jpg)

## Sesnsor info
- [Adafruit BME280](https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf)

# SW info

# Development Environment

## Prerequisities
- Python 3.7+
- [Git](https://git-scm.com/download/win)
- [CP210x USB to UART Bridge VCP Drivers](https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers)
  - [Download VCP for Win 7](https://www.silabs.com/documents/public/software/CP210x_Windows_Drivers.zip)
- [Visual studio code](https://code.visualstudio.com/) (optional)
  - [Setting up Visual Studio Code for ESP32 IDF (FreeRTOS)](https://github.com/Deous/VSC-Guide-for-esp32) (Contains references and advices for older version of ESP-IDF)
- [Putty](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)

## Install toolchain and libraries
1. Clone this project somewhere e.g. `./cima-esp32-azure/`
2. Create separate ESP toolchain and libraries folder e.g. `./ESP/`
```
    mkdir ./ESP/
    cd ./ESP/
```
3. Clone _ESP-IDF_ to folder from step 2. e.g. `./ESP/esp-idf/`
```
    git clone -b release/v4.0 --recursive https://github.com/espressif/esp-idf.git
```
4. Clone _ESP-Azure_ to folder from step 2. e.g. `./ESP/esp-azure/`
```
    git clone --recursive https://github.com/espressif/esp-azure.git
    git submodule update --init --recursive
```
5. Clone _ESP-IoT-solution_ from fork _cima/esp-iot-solution_ to folder from step 2. e.g. `./ESP/esp-iot-solution/`
```
    git clone -b feature/ssd1306_generic --recursive https://github.com/cima/esp-iot-solution.git
```
6. get Boost via [Version 1.71.0](https://www.boost.org/users/history/version_1_71_0.html) and unpack it to folder from step 2. E.g. `./ESP/boost_1_71_0/`
7. ESP 32 toolchain v4.0 (incl. gcc, cmake, ninja) -- Windows
Download [esp-idf-tools-setup-2.3.exe](https://dl.espressif.com/dl/esp-idf-tools-setup-2.3.exe) see [Standard Setup of Toolchain for Windows](https://docs.espressif.com/projects/esp-idf/en/release-v4.0/get-started/windows-setup.html) for more details
  a. Select your preinstalled GIT and Python
  b. When asked whether to download or use ESP-IDF select _Use an existing ESP-IDF directory_ an navigate to the folder from step 3.
  c. When you asked _Select Destination Location_ you can also point to folder from step 2 to have all the toolchain at one place.

8. Update file `init_cmd.bat` in this project (from step 1) so the first variable `ESP_TOOLCHAIN_DIR` contains the absolute prefix of your toolchain directory.
9. Fisrt build
```
    init_cmd.bat
    idf.py build
```

## Development

### Build & Flash

```
    idf.py -p COM5 flash
```

### STDOUT over serial line over USB
1. Plug your Lily GO Oled to USB
2. Open Putty
  -  Connection Type: serial
  -  Serial Line _COM5_ (Based on where your board appears the number can differ)
  -  Speed 115200

### Wi-Fi Networks, Certificates

- (./filesystem/connectivity/wifi.json) populate with wi-fi networks where your ESP will operate
- (./filesystem/connectivity/azure.json) populate with your azure account adresses

_TODO: Certificates_
- (./filesystem/identity/)

## Dependencies details

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
and define environment variable named `AZURE_SDK_C_PATH` pointing to the above cloned directory.

- [Azure IoT Hub example](https://github.com/Azure/azure-iot-sdk-c/blob/350b51f5abaedc975dae5419ad1fa4add7635fd2/iothub_client/samples/iothub_ll_client_x509_sample/iothub_ll_client_x509_sample.c)

### BOOST
- [Getting started with boost](https://www.boost.org/doc/libs/1_71_0/more/getting_started/windows.html)
- Ensure environment variable *BOOST_ROOT* that contain path to unpacked boost distribution.

### Misc

- [Disable FreeRTOS static allocation API](https://esp32.com/viewtopic.php?t=3504) set in [sdkconfig.defaults](sdkconfig.defaults)
- [Quartz & Bitrate settings](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#get-started-connect)

### Adafruit drivers
Use [ESP IoT Solution](https://github.com/espressif/esp-iot-solution/) clone it with all submodules
```
    git clone --recursive https://github.com/espressif/esp-iot-solution.git
    git submodule update --init --recursive
```
and define environment variable named `IOT_SOLUTION_PATH` pointing to the above cloned directory.

In (./CMakeLists.txt) we use a little dirty trick where we only define and use those drives we care of because the rest is highly incompatible with ESP-IDF 4.0.

# Deprecated

## Arduino like
- clone with submodules https://github.com/espressif/esp-azure to c:/Users/z608328/AppData/Local/Arduino15/packages/esp32/hardware/esp32/1.0.1/libraries/esp-azure/
- Adafruit drivers for Arduino
  - Patch adafruit driver
