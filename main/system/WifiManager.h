#pragma once

#include <string>
#include <esp_event_base.h>
#include <esp_wifi.h>
#include "Log.h"

namespace cima::system {

class WifiManager {

    static Log LOG;
    static bool started;

    std::string ssid;
    std::string passphrase;

    wifi_init_config_t firmwareWifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifiConfig;

    int connectionAttempts = 0;

public:
    WifiManager(std::string &ssid, std::string &passphrase);

    void start();
    void initFlashStorage();
    bool isStarted();

    void wifiEventHandler(int32_t event_id, void* event_data);
    void ipEventHandler(int32_t event_id, void* event_data);

private:
    static void wifiEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ipEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    void initWifiStationConfig(const std::string &ssid, const std::string &password);
};

}