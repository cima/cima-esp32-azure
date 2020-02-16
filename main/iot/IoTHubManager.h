#pragma once

#include <iothub_device_client_ll.h>

#include <string>
#include <memory>
#include <map>
#include <chrono>
#include <functional>

#include "CertSource.h"
#include "../system/Log.h"
#include "../system/ExecutionLimiter.h"

namespace cima::iot {

    typedef std::function<int(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize)> IotHubClientMethod;

    class IoTHubManager {

        static ::cima::system::Log LOGGER;

        static const char *CONNECTION_STRING_TEMPLATE;

        ::cima::system::ExecutionLimiter limiter;

        const CertSource &certificate;
        std::string pemCertificate;
        std::string pemPrivateKey;

        /**
         * String containing Hostname, Device Id & Device Key in the format:                         
         *    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"               
         *    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"
        */
        std::string connectionString;

        const std::string &iotHubHostname;

        static void releaseIotHubHandle(IOTHUB_CLIENT_CORE_LL_HANDLE_DATA_TAG *iotHubHandle);
        std::unique_ptr<IOTHUB_CLIENT_CORE_LL_HANDLE_DATA_TAG, decltype(&releaseIotHubHandle)> iotHubClientHandle;

        std::map<std::string, IotHubClientMethod> methods;

        bool connected = false;

        bool stopFlag = false;

    public:
        IoTHubManager(const std::string &iotHubHostname, const CertSource &certificate);

        void setHostname(const std::string &iotHubHostname);
        void setIdentity(CertSource &certificate);
        void connect();
        void close();

        bool isReady();
        void sendMessage(const char *messagePayload);
        void loop();

        void registerMethod(const std::string &methodName, IotHubClientMethod method);

    private:
        void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, size_t size);
        IOTHUBMESSAGE_DISPOSITION_RESULT messageCallback(IOTHUB_MESSAGE_HANDLE message);
        int deviceMethodCallback(const char *methodName, const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize);
        void connectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS status, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason);

    private:
        static void sendConfirmationCallbackWrapper(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
        static void deviceTwinCallbackWrapper(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size, void* userContextCallback);
        static IOTHUBMESSAGE_DISPOSITION_RESULT messageCallbackWrapper(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback);
        static int deviceMethodCallbackWrapper(const char* methodName, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback);
        static void connectionStatusCallbackWrapper(IOTHUB_CLIENT_CONNECTION_STATUS status, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContext);
    };
}