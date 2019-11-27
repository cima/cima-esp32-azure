#include "WifiManager.h"
#include <functional>

namespace cima::system {

    bool WifiManager::started = false;

    WifiManager::WifiManager(std::string &ssid, std::string &passphrase) 
        : ssid(ssid), passphrase(passphrase), 
        logger(Log("WifiManager")){
    }

    void WifiManager::start(){
        logger.info("Connecting...");

       /* TODO 
        WiFi.begin(ssid, passphrase);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            logger.info(".");
        }
        */
        started = true;

        logger.info("WiFi connected");
        logger.info("IP address: ");
        //logger.info("%s", WiFi.localIP());
    }

    bool WifiManager::isStarted(){
        return started;
    }

}