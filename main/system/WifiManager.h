#pragma once

#include <string>
#include "Log.h"

namespace cima::system {

class WifiManager {

    static bool started;

    std::string ssid;
    std::string passphrase;

    Log logger;

public:
    WifiManager(std::string &ssid, std::string &passphrase);

    void start();
    bool isStarted();
};

}