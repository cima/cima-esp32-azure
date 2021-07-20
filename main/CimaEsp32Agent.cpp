#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <time.h>

#include <driver/gpio.h>
#include <iot_ssd1306.h>

#include <cJSON.h>

#include "system/Log.h"

#include "system/network/WifiManager.h"
#include "system/WireManager.h"
#include "system/EnvironmentSensorManager.h"
#include "system/ButtonController.h"
#include "system/PWMDriver.h"

#include "iot/CertSource.h"
#include "iot/AzureConfig.h"
#include "iot/IoTHubManager.h"
#include "iot/DeviceProvisioningClient.h"

#include "display/Display.h"
#include "display/StatusIcon.h"

#include "Agent.h"
#include "GreetingService.h"
#include "LightAlarmService.h"
#include "LightSettings.h"

cima::system::Log logger("main");

cima::system::network::WifiManager wifiManager;

cima::system::WireManager wireManager(OLED_IIC_NUM, GPIO_NUM_15, GPIO_NUM_4);
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);
cima::system::ButtonController buttonController(GPIO_NUM_0);

//TODO find certificate in folder instead of fixed path
std::string keyFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.pem";
std::string certFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.crt";
cima::iot::CertSource certificate(keyFile, certFile);

cima::iot::DeviceProvisioningClient dpsClient(certificate);

cima::Agent agent;

bool azureConnected;
std::shared_ptr<cima::iot::IoTHubManager> iotHubManagerPtr;

cima::display::BooleanStatusIcon wifiStatusIcon(cima::display::StatusIcon::ICON_WIFI_88);
cima::display::BooleanStatusIcon azureStatusIcon(cima::display::StatusIcon::ICON_AZURE_88);

cima::system::PWMDriver mosfetDriver(GPIO_NUM_27);

std::string lightScheduleFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/lightSchedule.json";
cima::LightSettings lightSettings;
cima::LightAlarmService lightAlarmService(mosfetDriver);

extern "C" void app_main(void)
{
  using namespace std::placeholders;

  logger.init();
  logger.info("ESP32 Device");
  logger.info("Initializing...");

  agent.initFlashStorage();

  if(agent.mountFlashFileSystem()){
    agent.cat("/spiffs/sheep.txt");
  }

  logger.info(" > Light settings");
  lightSettings.updateFromFile(lightScheduleFile);

  cima::display::Display display(wireManager, cima::display::Display::LILYGO_OLED_CONFIG);
  display.addStatusIcon((cima::display::StatusIcon *)&wifiStatusIcon);
  display.addStatusIcon((cima::display::StatusIcon *)&azureStatusIcon);
  agent.registerToMainLoop(
    [&]() {
      azureStatusIcon.setActive(iotHubManagerPtr && iotHubManagerPtr->isReady());//TODO make it asynch over up/down handlers
      display.showTemperature(environmentSensorManager.readTemperature(), 
      environmentSensorManager.readHumidity(),
      environmentSensorManager.readPressure());
    });

  agent.setupNetwork(wifiManager);

  
  logger.info(" > Azure config file");
  auto azureConfig = agent.readAzureConfig();


  logger.info(" > DPS client");
  cima::iot::DeviceProvisioningClient::initSingleton(&dpsClient);
  dpsClient.connect();
  

  logger.info(" > IoT Hub");
  logger.info("Common name is: %s", certificate.getCommonName().c_str());

  logger.info("IoT hub Hostname: %s", azureConfig.getIotHubHostname().c_str());


  std::shared_ptr<cima::iot::IoTHubManager> iotHubManager(new cima::iot::IoTHubManager(azureConfig.getIotHubHostname(), certificate));
  iotHubManagerPtr = iotHubManager;
  
  iotHubManager->registerMethod("justPrint", std::bind(&cima::Agent::justPrint, &agent, 
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  
  iotHubManager->registerMethod("whatIsTheTime", std::bind(&cima::Agent::whatIsTheTime, &agent, 
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  iotHubManagerPtr->registerDeviceTwinListener("SunriseAlarm", [&](const char *rawDeviceTwin){
    logger.info("Device twin within callback");
    cJSON *root = cJSON_Parse(rawDeviceTwin);
    cJSON *desired = cJSON_GetObjectItem(root, "desired");
    cJSON *light = cJSON_GetObjectItem(desired, "light");

    char *lightSchedule = cJSON_Print(light);

    lightSettings.updateFromJson(lightSchedule);

    cJSON_free(root);
    cJSON_free(lightSchedule);
  });

  agent.registerToMainLoop([&](){iotHubManager->loop();});
  
  wifiManager.registerNetworkUpHandler([&](){iotHubManager->connect();});
  wifiManager.registerNetworkDownHandler([&](){iotHubManager->close();});
  
  wifiManager.registerNetworkUpHandler([&](){wifiStatusIcon.setActive(true);});
  wifiManager.registerNetworkDownHandler([&](){wifiStatusIcon.setActive(false);});

  wifiManager.start();

  logger.info(" > Environment sensor");
  environmentSensorManager.init();

  logger.info("Temp: %f, hum: %f", 
    environmentSensorManager.readTemperature(), 
    environmentSensorManager.readHumidity());

  std::shared_ptr<cima::GreetingService> greetingService(new cima::GreetingService(*iotHubManager, environmentSensorManager));

  std::string gizmo("Gizmo");
  auto welcomeGizmo = std::thread(&cima::GreetingService::welcomeLoop, std::ref(*greetingService), std::ref(gizmo));

  std::string sheep("Sheep");
  auto welcomeSheep = std::thread(&cima::GreetingService::welcomeLoop, std::ref(*greetingService), std::ref(sheep));

  std::string button("Button");
  auto buttonFunc = std::bind(&cima::GreetingService::welcome, greetingService, std::ref(button));
  
  logger.info("Test calling the button handler");
  buttonFunc();

  logger.info("Registering button handler");
  buttonController.initButton();
  buttonController.addHandler(buttonFunc);
  agent.registerToMainLoop(std::bind(&cima::system::ButtonController::handleClicks, &buttonController));
  
  //agent.registerToMainLoop([&](){ lightAlarmService.loop(); });
  auto lightAlarmServiceThread = std::thread([&](){ lightAlarmService.loop(); });

  logger.info(" > Main loop");
  agent.mainLoop();

  welcomeGizmo.join();
  welcomeSheep.join();
  lightAlarmServiceThread.join();

}
