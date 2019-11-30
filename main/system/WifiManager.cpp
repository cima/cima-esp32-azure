#include "WifiManager.h"
#include <functional>
#include <cstring>

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include <lwip/err.h>
#include <lwip/sys.h>

#define EXAMPLE_ESP_MAXIMUM_RETRY  10

namespace cima::system {

    Log WifiManager::LOG("WifiManager");

    WifiManager::WifiManager(const std::string &ssid, const std::string &passphrase) 
        : started(false), connected(false), ssid(ssid), passphrase(passphrase) {
    }

    void WifiManager::start(){

        initFlashStorage();

        LOG.info("Connecting...");

        tcpip_adapter_init();

        ESP_ERROR_CHECK(esp_event_loop_create_default());
        ESP_ERROR_CHECK(esp_wifi_init(&firmwareWifiConfig));

        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiManager::wifiEventHandlerWrapper, this));
        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiManager::ipEventHandlerWrapper, this));

        initWifiStationConfig(ssid, passphrase);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifiConfig) );
        ESP_ERROR_CHECK(esp_wifi_start());
        started = true;

        LOG.info("Wifi station started.");
    }

    void WifiManager::initFlashStorage(){
        LOG.info("Initializing flash storage...");

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
    }

    void WifiManager::wifiEventHandler(int32_t event_id, void* event_data) {
        if (event_id == WIFI_EVENT_STA_START) {
            auto connectresult = esp_wifi_connect();
            LOG.info("Connection attempt %d ended with code: %d", connectionAttempts, connectresult);
        } else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            if (connectionAttempts < EXAMPLE_ESP_MAXIMUM_RETRY) {
                auto connectresult = esp_wifi_connect();
                LOG.info("Connection attempt %d ended with code: %d", connectionAttempts, connectresult);

                auto kickReason = (wifi_event_sta_disconnected_t *)event_data;
                if(kickReason) {
                    LOG.info("Kick reason: %d", kickReason->reason);
                }

                connectionAttempts++;
                LOG.info("retry to connect to the AP");
            } else {
                LOG.info("Connectin to the AP has failed");
            }
            connected = false;
        }
    }

    void WifiManager::ipEventHandler(int32_t event_id, void* event_data) {
        if (event_id == IP_EVENT_STA_GOT_IP) {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            LOG.info("got ip:" IPSTR, IP2STR(&event->ip_info.ip));
            connectionAttempts = 0;
            connected = true;
        }
    }

    bool WifiManager::isStarted(){
        return started;
    }

    bool WifiManager::isConnected(){
        return connected;
;
    }

    void WifiManager::wifiEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            ((WifiManager *)arg)->wifiEventHandler(event_id, event_data);
    }

    void WifiManager::ipEventHandlerWrapper(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
            ((WifiManager *)arg)->ipEventHandler(event_id, event_data);
    }

    void WifiManager::initWifiStationConfig(const std::string &ssid, const std::string &password) {
        
        wifi_sta_config_t &wifi_config_sta = wifiConfig.sta;
        std::strcpy((char *)wifi_config_sta.ssid,ssid.c_str());
        std::strcpy((char *)wifi_config_sta.password, password.c_str());

        wifi_config_sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
        wifi_config_sta.bssid_set = false;
        wifi_config_sta.channel = 0;
        wifi_config_sta.listen_interval = 0;
        wifi_config_sta.sort_method = WIFI_CONNECT_AP_BY_SECURITY; 

        LOG.info("Setup to connect to AP SSID:%s password:%s", (char *)wifiConfig.sta.ssid, (char *)wifiConfig.sta.password);
    }
}