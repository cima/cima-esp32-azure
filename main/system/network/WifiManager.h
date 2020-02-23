#pragma once

#include <string>
#include <list>
#include <iterator>

#include <esp_event_base.h>
#include <esp_wifi.h>

#include <boost/signals2/signal.hpp>

#include "../Log.h"
#include "WifiCredentials.h"
#include "NetworkManager.h"

namespace cima::system::network {

class WifiManager : public NetworkManager {

    static Log LOG;

    bool started;

    std::list<WifiCredentials> credentials;

    wifi_init_config_t firmwareWifiConfig = WIFI_INIT_CONFIG_DEFAULT();
    wifi_config_t wifiConfig;

    int connectionAttempts = 0;

    std::list<WifiCredentials> networks;
    std::list<WifiCredentials>::iterator networkIterator;

    boost::signals2::signal<void ()> networkUpSignal;
    boost::signals2::signal<void ()> networkDownSignal;

public:
    WifiManager();

    void resetNetwork(const std::string &ssid, const std::string &passphrase);
    void addNetwork(const WifiCredentials &credentials);

    void start();
    void initAccesspoint();
    void tryNextNetwork();

    bool isStarted();
    virtual bool isConnected();

private:
    void wifiEventHandler(int32_t event_id, void* event_data);
    void ipEventHandler(int32_t event_id, void* event_data);

private:
    static void wifiEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static void ipEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

    void initWifiStationConfig(const std::string &ssid, const std::string &password);
};

}