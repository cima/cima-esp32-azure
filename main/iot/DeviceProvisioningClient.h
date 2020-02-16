#pragma once
#include <string>
#include <hsm_client_data.h>

#include "CertSource.h"

namespace cima::iot {
    class DeviceProvisioningClient {

        static DeviceProvisioningClient *globalSingleton;
        static HSM_CLIENT_X509_INTERFACE callbacks;

        const CertSource &certSource;

        std::string certificate;
        std::string privateKey;
        std::string commonName;

    public:
        DeviceProvisioningClient(const CertSource &certSource);
        void connect();

        const static HSM_CLIENT_X509_INTERFACE *getAzureHsmCallbacks();

    private:
        HSM_CLIENT_HANDLE create();
        void destroy();

        const char *getCertificate();
        const char *getPrivateKey();
        const char *getCommonName();
    public:
            /**
         * Sets the static globalSingleton to an object provided in argument and treturns previous value of the globalSingleton
         */
        static DeviceProvisioningClient *initSingleton(DeviceProvisioningClient *object);
    private:

        static HSM_CLIENT_HANDLE createStatic();
        static void destroyStatic(HSM_CLIENT_HANDLE handle);

        static char* getCertificateStatic(HSM_CLIENT_HANDLE handle);
        static char* getPrivateKeyStatic(HSM_CLIENT_HANDLE handle);
        static char* getCommonNameStatic(HSM_CLIENT_HANDLE handle);

    };
}

/**
 *  An oldcshool way how to provide implementation of an interface to a target call.
 * Very static. Totally not objective. Shame on you architect.
 * 
 * NOTE: must be extern "C" & Must be in root namespace. Awful. :-ú
 */
extern "C" const HSM_CLIENT_X509_INTERFACE *hsm_client_x509_interface();