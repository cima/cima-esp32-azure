#include "AzureConfig.h"

namespace cima::iot {
    const std::string &AzureConfig::getDpsHostname() const {
        return dpsHostname;
    }

    void AzureConfig::setDpsHostname(const std::string &dpsHostname) {
        this->dpsHostname = dpsHostname;
    }

    const std::string &AzureConfig::getDpsScopeId() const {
        return dpsScopeId;
    }

    void AzureConfig::setDpsScopeId(const std::string &scopeId) {
        this->dpsScopeId = scopeId;
    }

    const std::string &AzureConfig::getIotHubHostname() const {
        return iotHubHostname; 
    }

    void AzureConfig::setIotHubHostname(const std::string &iotHubHostname) {
        this->iotHubHostname = iotHubHostname;
    }
}