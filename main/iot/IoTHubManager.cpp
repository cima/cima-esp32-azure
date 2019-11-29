#include "IoTHubManager.h"

#include <string>
#include <thread>
#include <chrono>
#include<functional>


#include <iothub.h>
#include <iothub_message.h>
#include <iothub_client_options.h>
#include <iothub_client_core_common.h>
#include <azure_c_shared_utility/threadapi.h>
#include <azure_c_shared_utility/shared_util_options.h>
#include <azure_c_shared_utility/platform.h>

#include <iothubtransportmqtt.h> //TODO request AMQP support in esp-azure

namespace cima::iot {

    ::cima::system::Log IoTHubManager::LOGGER("IoTHubManager");

    IoTHubManager::IoTHubManager(const std::string &connectionString, const CertSource &certificate) 
        : connectionString(connectionString),
        certificate(certificate)
        , device_ll_handle(nullptr, &IoTHubManager::releaseIotHubHandle) 
        {}

    void IoTHubManager::init() {
        int result = IoTHub_Init();
        if(result){
             LOGGER.error("Init of IoT hub platform failed");
             return;
        }
    
        device_ll_handle.reset(IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString.c_str(), MQTT_Protocol));

/* TODO zjistit co to je
#ifdef SET_TRUSTED_CERT_IN_SAMPLES
        IoTHubDeviceClient_SetOption(device_ll_handle.get(), OPTION_TRUSTED_CERT, certificates);
#endif
*/

        if ( ! device_ll_handle ||
            (IoTHubDeviceClient_LL_SetOption(device_ll_handle.get(), OPTION_X509_CERT, certificate.getPemCertificate().c_str()) != IOTHUB_CLIENT_OK) ||
            (IoTHubDeviceClient_LL_SetOption(device_ll_handle.get(), OPTION_X509_PRIVATE_KEY, certificate.getPemPrivateKey().c_str()) != IOTHUB_CLIENT_OK)
        ){
            LOGGER.error("failure to set options for x509, aborting");
            device_ll_handle.reset();
            return;
        } 

        // Device Twin update callback
        auto deviceTwinsCallbackBindResult = IoTHubDeviceClient_LL_SetDeviceTwinCallback(device_ll_handle.get(), &IoTHubManager::deviceTwinCallbackWrapper, this);
        if(deviceTwinsCallbackBindResult != IOTHUB_CLIENT_OK){
            LOGGER.error("Binding of callback for device twin update failed. %s", 
            IOTHUB_CLIENT_RESULTStrings(deviceTwinsCallbackBindResult));
        }

        // Message callback to get C2D messages
        auto messageCallbackBindResult = IoTHubDeviceClient_LL_SetMessageCallback(device_ll_handle.get(), &IoTHubManager::messageCallbackWrapper, this);
        if(messageCallbackBindResult != IOTHUB_CLIENT_OK){
            LOGGER.error("Binding of callback for Cloud to device messages failed. %s",
            IOTHUB_CLIENT_RESULTStrings(messageCallbackBindResult));
        }

        // Method callback
        auto deviceMethodCallbackBindResult = IoTHubDeviceClient_LL_SetDeviceMethodCallback(device_ll_handle.get(), &IoTHubManager::deviceMethodCallbackWrapper, this);
        if(deviceMethodCallbackBindResult != IOTHUB_CLIENT_OK){
            LOGGER.error("Binding of callback for device method failed. %s",
            IOTHUB_CLIENT_RESULTStrings(deviceMethodCallbackBindResult));
        }

        // Connection status callback to get indication of connection to iothub
        auto connectionStatusBindResult = IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle.get(), &IoTHubManager::connectionStatusCallbackWrapper, this);
        if(connectionStatusBindResult != IOTHUB_CLIENT_OK){
            LOGGER.error("Binding of callback for connectivity status failed. %s", 
            IOTHUB_CLIENT_RESULTStrings(connectionStatusBindResult));
        }

        /* TODO
            Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
        */
    }

    bool IoTHubManager::isReady() {
        return (bool)device_ll_handle;
    }

    void IoTHubManager::sendMessage(const char *messagePayload) {
        IOTHUB_MESSAGE_HANDLE message_handle = IoTHubMessage_CreateFromString(messagePayload);
      
        (void)IoTHubMessage_SetMessageId(message_handle, "MSG_ID");
        (void)IoTHubMessage_SetCorrelationId(message_handle, "CORE_ID");
        (void)IoTHubMessage_SetContentTypeSystemProperty(message_handle, "application%2Fjson");
        (void)IoTHubMessage_SetContentEncodingSystemProperty(message_handle, "utf-8");

        (void)IoTHubMessage_SetProperty(message_handle, "temperatureAlert", "true");
        (void)IoTHubMessage_SetProperty(message_handle, "type", "CimaEvironmentMessage");

        //TODO tady to chce nějakou přepravku abych přihodil ještě kontext zprávy
        auto sendConfirmationCallbackBindResult = IoTHubDeviceClient_LL_SendEventAsync(device_ll_handle.get(), message_handle, sendConfirmationCallbackWrapper, this);
        if(sendConfirmationCallbackBindResult != IOTHUB_CLIENT_OK){
            LOGGER.error("Binding of callback for connectivity status failed. %s", 
            IOTHUB_CLIENT_RESULTStrings(sendConfirmationCallbackBindResult));
        }

        // The message is copied to the sdk so the we can destroy it
        IoTHubMessage_Destroy(message_handle);
    }

    void IoTHubManager::loop() {
        while( ! stopFlag) {
            IoTHubDeviceClient_LL_DoWork(device_ll_handle.get());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void IoTHubManager::stop(){
        stopFlag = true;
    }

    // BEGIN ---------- Send confirmation callback section ----------------
    void IoTHubManager::sendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result) {
        if (result == IOTHUB_CLIENT_CONFIRMATION_OK) {
            LOGGER.info("Message sent. Confirmed.");
        }
        LOGGER.info("Confirmation callback received for message with result %s",
            IOTHUB_CLIENT_CONFIRMATION_RESULTStrings(result));
    }
    

    void IoTHubManager::sendConfirmationCallbackWrapper(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback) {
        ((IoTHubManager *)userContextCallback)->sendConfirmationCallback(result);
    }
    // END ---------- Send confirmation callback section ----------------

    // BEGIN ---------- Device Twin callback section ------------------------
    void IoTHubManager::deviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, size_t size) {
        char temp[size + 1];

        memcpy(temp, payLoad, size);
        temp[size] = '\0';

        LOGGER.info("Device twin: %s", temp);
    }

    void IoTHubManager::deviceTwinCallbackWrapper(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad, size_t size, void* userContextCallback) {
        ((IoTHubManager *)userContextCallback)->deviceTwinCallback(updateState, payLoad, size);
    }
    // END ---------- Device Twin callback section ------------------------

    // BEGIN ------------ incomming message callbacks section -----------------
    IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubManager::messageCallback(IOTHUB_MESSAGE_HANDLE message) {
        const char *messageId = IoTHubMessage_GetMessageId(message);

        LOGGER.info("Incomming Message: %s", messageId ? messageId : "N/A");
        
        //TODO process incomming message. E.g. pass it ot registered observers.
        return IOTHUBMESSAGE_REJECTED;
    }

    IOTHUBMESSAGE_DISPOSITION_RESULT IoTHubManager::messageCallbackWrapper(IOTHUB_MESSAGE_HANDLE message, void* userContextCallback) {
        return ((IoTHubManager *)userContextCallback)->messageCallback(message);
    }
    // END ------------ incomming message callbacks section -----------------

    // BEGIN ---------- device method callbyck section ----------------------
    int IoTHubManager::deviceMethodCallback(const char *methodName, const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize) {

        //TODO tohle je z nějakýho examplu, tady se to má jenom přehazovat do registrovanejch observerů
        LOGGER.info("Try to invoke method %s", methodName);
        const char *responseMessage = "\"Successfully invoke device method\"";
        int result = 200;

        if (strcmp(methodName, "start") == 0)
        {
            LOGGER.info("Start sending temperature and humidity data");
            messageSending = true;
        }
        else if (strcmp(methodName, "stop") == 0)
        {
            LOGGER.info("Stop sending temperature and humidity data");
            messageSending = false;
        }
        else
        {
            LOGGER.info("No method %s found", methodName);
            responseMessage = "\"No method found\"";
            result = 404;
        }

        *responseSize = strlen(responseMessage) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return result;
    }

    int IoTHubManager::deviceMethodCallbackWrapper(const char* method_name, const unsigned char* payload, size_t size, unsigned char** response, size_t* responseSize, void* userContextCallback){
        return ((IoTHubManager *)userContextCallback)->deviceMethodCallback(method_name, payload, size, response, responseSize);
    }
    // END ---------- device method callbyck section ----------------------

    // BEGIN -------- connection status callback section ------------------
    void IoTHubManager::connectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS status, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason){
        LOGGER.info("IoT hub connection status: %s (%s)",
         status == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED ? "Authenticated" : "Not-Authenticated", 
         IOTHUB_CLIENT_CONNECTION_STATUS_REASONStrings(reason));
    }

    void IoTHubManager::connectionStatusCallbackWrapper(IOTHUB_CLIENT_CONNECTION_STATUS status, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void* userContext){
        ((IoTHubManager *)userContext)->connectionStatusCallback(status, reason);
    }
    // END -------- connection status callback section ------------------

    void IoTHubManager::releaseIotHubHandle(IOTHUB_CLIENT_CORE_LL_HANDLE_DATA_TAG *iotHubHandle){
        if(iotHubHandle){
            IoTHubDeviceClient_LL_Destroy(iotHubHandle);
        }
    }
}