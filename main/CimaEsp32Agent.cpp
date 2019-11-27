#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "system/Log.h"

#include "system/WifiManager.h"
#include "system/WireManager.h"
#include "system/EnvironmentSensorManager.h"

#include "iot/CertSource.h"
#include "iot/IoTHubManager.h"

#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>

cima::system::Log logger("main");

std::string ssid     = "<your wifi SSID here>";
std::string password = "<your wifi password here>";

cima::system::WifiManager wifiManager(ssid, password);
cima::system::WireManager wireManager(4, 15);
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);

cima::iot::CertSource certificate;
std::string connectionString("HostName=daedalus-iot-CDO-1.azure-devices.net");

cima::iot::IoTHubManager iotHubManager(connectionString, certificate);


#define SENDING_INTERVAL 10000
#define DEVICE_ID "Esp32Device"
#define MESSAGE_MAX_LEN 256
#define STACK_SIZE 2048

const char *MESSAGE_TEMPLATE = "{\"greetings\":\"Hello world!\", \"Temperature\":%f, \"Humidity\":%f}";

void loop(void * pvParameters);
void hello(void * pvParameters);

static bool messageSending = true;
static uint64_t last_send_ms;
static TaskHandle_t xHandle = NULL;

int64_t millis(){
  int64_t time_since_boot = esp_timer_get_time();
  return time_since_boot;
}

extern "C" void app_main(void)
{

  logger.init();
  logger.info("ESP32 Device");
  logger.info("Initializing...");

  logger.info(" > WiFi");
  wifiManager.start();
  
  logger.info(" > IoT Hub");
  //iotHubManager.init(); //TODO

  logger.info(" > Wire");
  //wireManager.init(); //TODO

  logger.info(" > Environment sensor");
  //environmentSensorManager.init(); //TODO

  //TODO looping thread
  
  xTaskCreate(hello, "Hello G", STACK_SIZE, (char *)"Gizmo", tskIDLE_PRIORITY, &xHandle);
  xTaskCreate(hello, "Hello S", STACK_SIZE, (char *)"Sheep", tskIDLE_PRIORITY, NULL);
  

  last_send_ms = millis();

  //TODO co to je?
  //randomSeed(analogRead(0));
}

void hello(void * pvParameters) {
  while(true){
    logger.info(":-) Hello %s :-)", (char *)pvParameters);
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}

void loop(void * pvParameters) {
  if (wifiManager.isStarted()) {
    if (messageSending && 
        (millis() - last_send_ms) >= SENDING_INTERVAL &&
        iotHubManager.isReady())
    {
      // Send environmental data
      char messagePayload[MESSAGE_MAX_LEN];
      float temperature = environmentSensorManager.readTemperature();
      float humidity = environmentSensorManager.readHumidity();
      snprintf(messagePayload, MESSAGE_MAX_LEN, MESSAGE_TEMPLATE, temperature,humidity);
      logger.info(messagePayload);
      iotHubManager.sendMessage(messagePayload);

      iotHubManager.loop();

      last_send_ms = millis();
    }
    else
    {
      //Esp32MQTTClient_Check();
    }
  }

  //This should be 10 seconds -> make it named configurable variable with units
  usleep(10000000);
}