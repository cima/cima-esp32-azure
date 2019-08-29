#pragma once

#include <string>

#include <iothub_device_client_ll.h>

#include "CertSource.h"
#include "../system/Log.h"

namespace cima::iot {
    class IoTHubManager {

        /**
         * String containing Hostname, Device Id & Device Key in the format:                         
         *    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"               
         *    "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"
         *    "HostName=daedalus-iot-CDO-1.azure-devices.net;DeviceId=cima-esp32;SharedAccessKey=rgR/7coGtZn+5/7xHk3zrTHQ/5WwGuBj5QK9gmHqUl4=";
        */
        std::string connectionString;

        CertSource certificate;

        IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;

        ::cima::system::Log logger("IoTHubManager");

    public:
        IoTHubManager(std::string &connectionString, CertSource &certificate);
        void init();
        bool isReady();
        void sendMessage(const char *messagePayload);
        void loop();

        void sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result);
        void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback);
        void messageCallback(const char* payLoad, int size);
        void deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size);
        int deviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size);

    };
}