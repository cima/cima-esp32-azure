#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <time.h>

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <iot_ssd1306.h>

#include <cJSON.h>

#include "system/Log.h"

#include "system/network/WifiManager.h"
#include "system/network/Rf433Controller.h"
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
#include "LightGroupService.h"
#include "LightSettings.h"

cima::system::Log logger("main");

cima::system::network::WifiManager wifiManager;

cima::system::WireManager wireManager(OLED_IIC_NUM, GPIO_NUM_15, GPIO_NUM_4);
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);
cima::system::ButtonController buttonController(GPIO_NUM_0);
cima::system::network::Rf433Controller rf433Controller(GPIO_NUM_16);

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

const gpio_num_t WARM_LIGHT_MOSFET_DRIVER_GPIO = GPIO_NUM_26;
cima::system::PWMDriver warmLightMosfetDriver(WARM_LIGHT_MOSFET_DRIVER_GPIO, LEDC_CHANNEL_0, true);

const gpio_num_t COLD_LIGHT_MOSFET_DRIVER_GPIO = GPIO_NUM_27;
cima::system::PWMDriver coldLightMosfetDriver(COLD_LIGHT_MOSFET_DRIVER_GPIO, LEDC_CHANNEL_1, true);

std::string coldLightScheduleFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/coldLightSchedule.json";
std::string warmLightScheduleFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/warmLightSchedule.json";

std::string coldLightDeviceTwinName = "cold";
std::string warmLightDeviceTwinName = "warm";

cima::LightSettings coldLightSettings(COLD_LIGHT_MOSFET_DRIVER_GPIO);
cima::LightGroupService coldLightGroupService(coldLightMosfetDriver, coldLightSettings);

cima::LightSettings warmLightSettings(WARM_LIGHT_MOSFET_DRIVER_GPIO);
cima::LightGroupService warmLightGroupService(warmLightMosfetDriver, warmLightSettings);

cima::LightGroupMap lightGroups {
  {coldLightDeviceTwinName, boost::ref(coldLightGroupService)},
  {warmLightDeviceTwinName, boost::ref(warmLightGroupService)}
};

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
  coldLightSettings.updateFromFile(coldLightScheduleFile);
  warmLightSettings.updateFromFile(warmLightScheduleFile);

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

  iotHubManager->registerMethod("sineLight", std::bind(&cima::Agent::sineLight, &agent, lightGroups, 
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  iotHubManagerPtr->registerDeviceTwinListener("SunriseAlarm", [&](const char *rawDeviceTwin){
    logger.info("Device twin within callback");
    cJSON *root = cJSON_Parse(rawDeviceTwin);
    if( ! root) {
      logger.error("Cant parse device twin from: %s", rawDeviceTwin);
      return;
    }

    cJSON *desired = cJSON_GetObjectItem(root, "desired");
    if( ! desired) {
      //TODO do they really send such stupid once this once that?
      logger.info("Probably direct desired content");
      desired = root;
    }

    cJSON *lights = cJSON_GetObjectItem(desired, "lights");
    if( ! lights || ! cJSON_IsArray(lights)) {
      logger.info("Can't parse lights settings. Maybe none sent or not an array.");
      return;
    }
  
    int lightCount = cJSON_GetArraySize(lights);
    for (int i = 0; i < lightCount; i++) {
      cJSON *light = cJSON_GetArrayItem(lights, i);

      //TODO sanitize light object
      auto lightGroupName = std::string(cJSON_GetStringValue(cJSON_GetObjectItem(light, "name")));
      logger.info("Processing light group[%d]: %s", i, lightGroupName.c_str());

      cima::LightGroupMap::iterator lightGroupIt = lightGroups.find(lightGroupName);
      if (lightGroupIt == lightGroups.end()) {
        continue; //Skipping unregistered light group definition
      }

      auto schedule = cJSON_GetObjectItem(light, "schedule");

      if( ! schedule) {
        logger.info("Invalid schedule entry in lightgroup %s", lightGroupName);
        continue;
      }

      char *lightSchedule = cJSON_Print(schedule);

      auto lightGroup = lightGroupIt->second;

      lightGroup.get().setReady(true); //TODO this must be called on system time set (also after network up, but more generecially without azure)
      lightGroup.get().getLightSettings().updateFromJson(lightSchedule);

      cJSON_free(lightSchedule);
    }

    cJSON_free(root);

    //TODO just a test
    //lightAlarmService.loop(); //calls just 1x
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

  buttonController.addHandler([&](){ display.setEnabled( ! display.isEnabled()); });
  
  agent.registerToMainLoop(std::bind(&cima::system::ButtonController::handleClicks, &buttonController));

  logger.info("Registering rf433 handler");
  rf433Controller.initRf433();
  
  agent.registerToMainLoop(std::bind(&cima::system::network::Rf433Controller::handleData, &rf433Controller));
    
  for(auto lightGroupRef : lightGroups){
    agent.registerToMainLoop(std::bind(&cima::LightGroupService::loop, &lightGroupRef.second.get()));
  }

  logger.info(" > Main loop");
  agent.mainLoop();

  welcomeGizmo.join();
  welcomeSheep.join();
}
