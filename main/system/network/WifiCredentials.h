#pragma once
#include <string>

namespace cima::system::network {

class WifiCredentials{
    const std::string ssid;
    const std::string passphrase;

    public:
        WifiCredentials(const std::string &ssid, const std::string &passphrase);
        const std::string &getSsid();
        const std::string &getPassphrase();
};

}