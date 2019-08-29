#include "IoTHubManager.h"


//#include "AzureIotHub.h"
//#include "Esp32MQTTClient.h"

#include <iothub.h>
#include <iothub_client_options.h>
#include <iothub_message.h>
#include <azure_c_shared_utility/threadapi.h>
#include <azure_c_shared_utility/shared_util_options.h>

#include <iothubtransportmqtt.h>

namespace cima::iot {
    IoTHubManager::IoTHubManager(std::string &connectionString, CertSource &certificate) 
        : connectionString(connectionString), certificate(certificate) {}

    void IoTHubManager::init() {
        (void)IoTHub_Init();

        device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);

        if (
            device_ll_handle == NULL ||
            (IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_X509_CERT, certificate.getPemCertificate().c_str()) != IOTHUB_CLIENT_OK) ||
            (IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_X509_PRIVATE_KEY, certificate.getPemPrivateKey().c_str()) != IOTHUB_CLIENT_OK)
        ){
            logger.error("failure to set options for x509, aborting\r\n");
            IoTHubDeviceClient_LL_Destroy(device_ll_handle);
            device_ll_handle = NULL;

        } else {

        /*
            Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
            Esp32MQTTClient_SetMessageCallback(MessageCallback);
            Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
            Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
            */
        }
    }

    bool IoTHubManager::isReady() {
        return device_ll_handle != NULL;
    }

    void IoTHubManager::sendMessage(const char *messagePayload) {
        IOTHUB_MESSAGE_HANDLE message_handle = IoTHubMessage_CreateFromString(messagePayload);
      
        (void)IoTHubMessage_SetMessageId(message_handle, "MSG_ID");
        (void)IoTHubMessage_SetCorrelationId(message_handle, "CORE_ID");
        (void)IoTHubMessage_SetContentTypeSystemProperty(message_handle, "application%2Fjson");
        (void)IoTHubMessage_SetContentEncodingSystemProperty(message_handle, "utf-8");

        (void)IoTHubMessage_SetProperty(message_handle, "temperatureAlert", "true");
        (void)IoTHubMessage_SetProperty(message_handle, "type", "CimaEvironmentMessage");

        IoTHubDeviceClient_LL_SendEventAsync(device_ll_handle, message_handle, send_confirm_callback, NULL);

        // The message is copied to the sdk so the we can destroy it
        IoTHubMessage_Destroy(message_handle);
    }

    void IoTHubManager::loop() {
        IoTHubDeviceClient_LL_DoWork(device_ll_handle);
    }

    void IoTHubManager::sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result) {
        if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
            logger.info("Send Confirmation Callback finished.");
        }
    }

    void IoTHubManager::send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback) {
        (void)userContextCallback;
        logger.info("Confirmation callback received for message with result %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
    }

    void IoTHubManager::messageCallback(const char* payLoad, int size) { 
        logger.info("Message callback:");
        logger.info(payLoad);
    }

    void IoTHubManager::deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size) {
        char *temp = (char *)malloc(size + 1);
        if (temp == NULL) {
            return;
        }
        memcpy(temp, payLoad, size);
        temp[size] = '\0';
        // Display Twin message.
        logger.info(temp);
        free(temp);
    }

    int IoTHubManager::deviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size) {
        logger.info("Try to invoke method %s", methodName);
        const char *responseMessage = "\"Successfully invoke device method\"";
        int result = 200;

        if (strcmp(methodName, "start") == 0)
        {
            logger.info("Start sending temperature and humidity data");
            messageSending = true;
        }
        else if (strcmp(methodName, "stop") == 0)
        {
            logger.info("Stop sending temperature and humidity data");
            messageSending = false;
        }
        else
        {
            logger.info("No method %s found", methodName);
            responseMessage = "\"No method found\"";
            result = 404;
        }

        *response_size = strlen(responseMessage) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return result;
    }
}