#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <time.h>

#include <thread>
#include <chrono>
#include <memory>
#include <functional>

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

#include <driver/gpio.h>

cima::system::Log logger("main");

cima::system::WifiManager wifiManager;

cima::system::WireManager wireManager(GPIO_NUM_15, GPIO_NUM_4 );
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);


std::string keyFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.pem";
std::string certFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.crt";
cima::iot::CertSource certificate(keyFile, certFile);

/*  "HostName=<host_name>;DeviceId=<device_id>;x509=true"                      */
std::string connectionString("HostName=daedalus-iot-CDO-1.azure-devices.net;DeviceId=cimaesp32;x509=true");

cima::iot::IoTHubManager iotHubManager(connectionString, certificate);

cima::Agent agent(iotHubManager, environmentSensorManager);

#define SENDING_INTERVAL 10000
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
  using namespace std::placeholders;

  logger.init();
  logger.info("ESP32 Device");
  logger.info("Initializing...");

  if(agent.mountFlashFileSystem()){
    agent.cat("/spiffs/sheep.txt");
  }

  agent.setupNetwork(wifiManager);
  while( ! wifiManager.isConnected()){
    logger.info("Waiting for network");
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  logger.info(" > IoT Hub");
  iotHubManager.init();
  iotHubManager.registerMethod("justPrint", std::bind(&cima::Agent::justPrint, &agent, _1, _2, _3, _4));
  iotHubManager.registerMethod("whatIsTheTime", std::bind(&cima::Agent::whatIsTheTime, &agent, _1, _2, _3, _4));

  agent.registerToMainLoop(std::bind(&cima::iot::IoTHubManager::loop, &iotHubManager));

  logger.info(" > Environment sensor");
  environmentSensorManager.init();

  logger.info("Temp: %f, hum: %f", 
    environmentSensorManager.readTemperature(), 
    environmentSensorManager.readHumidity());

  std::string gizmo("Gizmo");
  std::string sheep("Sheep");

  auto welcomeGizmo = std::thread(&cima::Agent::welcome, std::ref(agent), std::ref(gizmo));
  auto welcomeSheep = std::thread(&cima::Agent::welcome, std::ref(agent), std::ref(sheep));

  last_send_ms = millis();

  agent.mainLoop();


  welcomeGizmo.join();
  welcomeSheep.join();

}
