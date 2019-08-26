#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include <time.h>

#include <iothub.h>
#include <iothub_device_client_ll.h>
#include <iothub_client_options.h>
#include <iothub_message.h>
#include <azure_c_shared_utility/threadapi.h>
#include <azure_c_shared_utility/shared_util_options.h>

#include <iothubtransportmqtt.h>

#include "system/WifiManager.h"
//#include "AzureIotHub.h"
//#include "Esp32MQTTClient.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "log/Log.h"
#include "certificates/CertSource.h"

cima::system::Log logger("main");

Adafruit_BME280 bme; // I2C

#define INTERVAL 10000
#define DEVICE_ID "Esp32Device"
#define MESSAGE_MAX_LEN 256

// Please input the SSID and password of WiFi
std::string ssid     = "<your wifi SSID here>";
std::string password = "<your wifi password here>";

/*String containing Hostname, Device Id & Device Key in the format:                         */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessKey=<device_key>"                */
/*  "HostName=<host_name>;DeviceId=<device_id>;SharedAccessSignature=<device_sas_token>"    */
//static const char* connectionString = "HostName=daedalus-iot-CDO-1.azure-devices.net;DeviceId=cima-esp32;SharedAccessKey=rgR/7coGtZn+5/7xHk3zrTHQ/5WwGuBj5QK9gmHqUl4=";
static const char* connectionString = "HostName=daedalus-iot-CDO-1.azure-devices.net";

static CertSource certificate;

const char *messageData = "{\"greetings\":\"Hello world!\", \"Temperature\":%f, \"Humidity\":%f}";

IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle;

int messageCount = 1;
cima::system::WifiManager wifiManager(ssid, password);

static bool messageSending = true;
static uint64_t send_interval_ms;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities


static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    logger.info("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{ 
  logger.info("Message callback:");
  logger.info(payLoad);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  logger.info(temp);
  free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
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

extern "C" void app_main(void)
{

  logger.init();
  logger.info("ESP32 Device");
  logger.info("Initializing...");
  logger.info("Deme na to!");

  // Initialize the WiFi module
  
  logger.info(" > WiFi");
  wifiManager.init();
  if ( ! wifiManager) {
    return;
  }
  randomSeed(analogRead(0));

  
  logger.info(" > IoT Hub");

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

  send_interval_ms = millis();

  Wire.begin(4,15);
  bme.begin(&Wire);  
}

void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void* userContextCallback)
{
  (void)userContextCallback;
  logger.info("Confirmation callback received for message with result %s\r\n", ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

void loop()
{
  if (hasWifi)
  {
    if (messageSending && 
        (int)(millis() - send_interval_ms) >= INTERVAL &&
        device_ll_handle != NULL)
    {
      // Send teperature data
      char messagePayload[MESSAGE_MAX_LEN];
      float temperature = (float)bme.readTemperature();
      float humidity = (float)bme.readHumidity();
      snprintf(messagePayload, MESSAGE_MAX_LEN, messageData, temperature,humidity);
      logger.info(messagePayload);


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

      IoTHubDeviceClient_LL_DoWork(device_ll_handle);

      send_interval_ms = millis();
    }
    else
    {
      //Esp32MQTTClient_Check();
    }
  }
  delay(10);
}