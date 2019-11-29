#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <time.h>
#include <thread>

#include "system/Log.h"

#include "system/WifiManager.h"
#include "system/WireManager.h"
#include "system/EnvironmentSensorManager.h"

#include "iot/CertSource.h"
#include "iot/IoTHubManager.h"

#include "Agent.h"

#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <freertos/task.h>

cima::system::Log logger("main");

std::string ssid     = "<your wifi SSID here>";
std::string password = "<your wifi password here>";

cima::Agent agent;

cima::system::WifiManager wifiManager(ssid, password);
cima::system::WireManager wireManager(4, 15);
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);

cima::iot::CertSource certificate;
std::string connectionString("HostName=daedalus-iot-CDO-1.azure-devices.net");

cima::iot::IoTHubManager iotHubManager(connectionString, certificate);

#define SENDING_INTERVAL 10000
#define DEVICE_ID "Esp32Device"
#define MESSAGE_MAX_LEN 256

const char *MESSAGE_TEMPLATE = "{\"greetings\":\"Hello world!\", \"Temperature\":%f, \"Humidity\":%f}";

static bool messageSending = true;
static uint64_t last_send_ms;

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
  auto wifiConnection = std::thread(&cima::system::WifiManager::start, std::ref(wifiManager));
  
  logger.info(" > IoT Hub");
  //iotHubManager.init(); //TODO

  logger.info(" > Wire");
  //wireManager.init(); //TODO

  logger.info(" > Environment sensor");
  //environmentSensorManager.init(); //TODO

  std::string gizmo("Gizmo");
  std::string sheep("Sheep");

  //TODO looping thread
  auto welcomeGizmo = std::thread(&cima::Agent::welcome, std::ref(agent), std::ref(gizmo));
  auto welcomeSheep = std::thread(&cima::Agent::welcome, std::ref(agent), std::ref(sheep));
  
  last_send_ms = millis();

  wifiConnection.join();
  welcomeGizmo.join();
  welcomeSheep.join();
  //TODO co to je?
  //randomSeed(analogRead(0));
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