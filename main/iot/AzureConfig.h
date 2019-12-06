#pragma once

#include <string>

namespace cima::iot {
    class AzureConfig {
        std::string dpsHostname;
        std::string dpsScopeId;

        std::string iotHubHostname;

    public:
        const std::string &getDpsHostname() const;
        void setDpsHostname(const std::string &dpsHostname);

        const std::string &getDpsScopeId() const;
        void setDpsScopeId(const std::string &scopeId);

        const std::string &getIotHubHostname() const;
        void setIotHubHostname(const std::string &iotHubHostname);
    };
}