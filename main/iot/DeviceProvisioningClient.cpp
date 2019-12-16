#include "DeviceProvisioningClient.h"

#include <memory>

namespace cima::iot {

    DeviceProvisioningClient *DeviceProvisioningClient::globalSingleton = nullptr;
    HSM_CLIENT_X509_INTERFACE DeviceProvisioningClient::callbacks;

    DeviceProvisioningClient::DeviceProvisioningClient(const CertSource &certSource) : certSource(certSource) {}

    const HSM_CLIENT_X509_INTERFACE *DeviceProvisioningClient::getAzureHsmCallbacks() {
        if( ! globalSingleton) {
            return nullptr;
        }

        return &callbacks;
    }

    DeviceProvisioningClient *DeviceProvisioningClient::initSingleton(DeviceProvisioningClient *object){
        DeviceProvisioningClient *oldSingleton = globalSingleton;
        globalSingleton = object;

        callbacks.hsm_client_x509_create = createStatic;
        callbacks.hsm_client_x509_destroy = destroyStatic;

        callbacks.hsm_client_get_cert = getCertificateStatic;
        callbacks.hsm_client_get_key = getPrivateKeyStatic;
        callbacks.hsm_client_get_common_name = getCommonNameStatic;

        return oldSingleton;
    }

    HSM_CLIENT_HANDLE DeviceProvisioningClient::create(){
        certificate = std::move(certSource.getPemCertificate());
        privateKey = std::move(certSource.getPemPrivateKey());
        commonName = std::move(certSource.getCommonName());

        return (HSM_CLIENT_HANDLE)this;
    }

    HSM_CLIENT_HANDLE DeviceProvisioningClient::createStatic(){
        if(globalSingleton){
            return globalSingleton->create();
        }
        return nullptr;
    }

    void DeviceProvisioningClient::destroy(){
        //TODO nothing
    }

    void DeviceProvisioningClient::destroyStatic(HSM_CLIENT_HANDLE handle){
        if(handle){
            ((DeviceProvisioningClient *)handle)->destroy();
        }
    }

    const char *DeviceProvisioningClient::getCertificate(){
        return certificate.c_str();
    }

    char* DeviceProvisioningClient::getCertificateStatic(HSM_CLIENT_HANDLE handle){
        if(handle){
            //FIXME disty const removal
            return (char *)((DeviceProvisioningClient *)handle)->getCertificate();
        }
        return nullptr;
    }
    
    const char *DeviceProvisioningClient::getPrivateKey(){
        return privateKey.c_str();
    }
     
    char* DeviceProvisioningClient::getPrivateKeyStatic(HSM_CLIENT_HANDLE handle){
        if(handle){
            //FIXME disty const removal
            return (char *)((DeviceProvisioningClient *)handle)->getPrivateKey();
        }
        return nullptr;
    }

    const char *DeviceProvisioningClient::getCommonName(){
        return commonName.c_str();
    }

    char* DeviceProvisioningClient::getCommonNameStatic(HSM_CLIENT_HANDLE handle){
        if(handle){
            //FIXME disty const removal
            return (char *)((DeviceProvisioningClient *)handle)->getCommonName();
        }
        return nullptr;
    }
}

extern "C" const HSM_CLIENT_X509_INTERFACE *hsm_client_x509_interface() {
    return cima::iot::DeviceProvisioningClient::getAzureHsmCallbacks();
}