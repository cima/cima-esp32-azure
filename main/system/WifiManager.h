#pragma once

#include <string>
#include <list>
#include <iterator>

#include <esp_event_base.h>
#include <esp_wifi.h>
#include "Log.h"
#include "WifiCredentials.h"

namespace cima::system {

class WifiManager {

    static Log LOG;
    bool started;
    bool connected;

    std::list<system::WifiCredentials> credentials;

    wifi_init_config_t firmwareWifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifiConfig;

    int connectionAttempts = 0;

    std::list<system::WifiCredentials> networks;
    std::list<system::WifiCredentials>::iterator networkIterator;

public:
    WifiManager();

    void resetNetwork(const std::string &ssid, const std::string &passphrase);
    void addNetwork(const system::WifiCredentials &credentials);

    void start();
    void initAccesspoint();
    void tryNextNetwork();

    void initFlashStorage();
    bool isStarted();
    bool isConnected();

    void wifiEventHandler(int32_t event_id, void* event_data);
    void ipEventHandler(int32_t event_id, void* event_data);

private:
    static void wifiEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ipEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    void initWifiStationConfig(const std::string &ssid, const std::string &password);
};

}