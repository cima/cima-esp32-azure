#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <functional>

#include <driver/gpio.h>

#include "system/Log.h"

#include "system/WifiManager.h"
#include "system/WireManager.h"
#include "system/EnvironmentSensorManager.h"
#include "system/ButtonController.h"

#include "iot/CertSource.h"
#include "iot/AzureConfig.h"
#include "iot/IoTHubManager.h"
#include "iot/DeviceProvisioningClient.h"

#include "Agent.h"
#include "GreetingService.h"

cima::system::Log logger("main");

cima::system::WifiManager wifiManager;

cima::system::WireManager wireManager(GPIO_NUM_15, GPIO_NUM_4 );
cima::system::EnvironmentSensorManager environmentSensorManager(wireManager);
cima::system::ButtonController buttonController(GPIO_NUM_0);

//TODO find certificate in folder instead of fixed path
std::string keyFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.pem";
std::string certFile = cima::Agent::FLASH_FILESYSTEM_MOUNT_PATH + "/identity/cimaesp32.crt";
cima::iot::CertSource certificate(keyFile, certFile);

cima::iot::DeviceProvisioningClient dpsClient(certificate);

cima::Agent agent;

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

  agent.setupNetwork(wifiManager);
  while( ! wifiManager.isConnected()){
    logger.info("Waiting for network");
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  
  logger.info(" > Azure config file");
  auto azureConfig = agent.readAzureConfig();

  logger.info(" > DPS client");
  cima::iot::DeviceProvisioningClient::initSingleton(&dpsClient);
  //dpsClient.connect() 

  logger.info(" > IoT Hub");
  logger.info("Common name is: %s", certificate.getCommonName().c_str());

  logger.info("IoT hub Hostname: %s", azureConfig.getIotHubHostname().c_str());

  std::shared_ptr<cima::iot::IoTHubManager> iotHubManager(new cima::iot::IoTHubManager(azureConfig.getIotHubHostname(), certificate));

  iotHubManager->connect();
  
  iotHubManager->registerMethod("justPrint", std::bind(&cima::Agent::justPrint, &agent, 
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
  
  iotHubManager->registerMethod("whatIsTheTime", std::bind(&cima::Agent::whatIsTheTime, &agent, 
    std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

  agent.registerToMainLoop(std::bind(&cima::iot::IoTHubManager::loop, iotHubManager.get()));

  logger.info(" > Environment sensor");
  environmentSensorManager.init();

  logger.info("Temp: %f, hum: %f", 
    environmentSensorManager.readTemperature(), 
    environmentSensorManager.readHumidity());

  std::shared_ptr<cima::GreetingService> greetingService(new cima::GreetingService(*iotHubManager, environmentSensorManager));

  std::string gizmo("Gizmo");
  auto welcomeGizmo = std::thread(&cima::GreetingService::welcomeLoop, std::ref(*greetingService), std::ref(gizmo));

  std::string sheep("Sheep");
  //auto welcomeSheep = std::thread(&cima::GreetingService::welcomeLoop, std::ref(*greetingService), std::ref(sheep));
  auto welcomeSheep = std::thread(&cima::GreetingService::welcomeLoop, std::ref(*greetingService), std::ref(sheep));

  std::string button("Button");
  auto buttonFunc = std::bind(&cima::GreetingService::welcome, greetingService, std::ref(button));
  logger.info("Test calling the button handler");
  buttonFunc();
  logger.info("Registering button handler");
  buttonController.initButton();
  buttonController.addHandler(buttonFunc);
  agent.registerToMainLoop(std::bind(&cima::system::ButtonController::handleClicks, &buttonController));

  logger.info(" > Main loop");
  
  auto mainLoopThread = std::thread(&cima::Agent::mainLoop, std::ref(agent));

  mainLoopThread.join();
  welcomeGizmo.join();
  welcomeSheep.join();

}
