#include "Agent.h"

#include <thread>
#include <chrono>
#include <ctime>  
#include <string>
#include <cstring>
#include <utility>
#include <memory>
#include <fstream>

#include <stdio.h>
#include <cJSON.h>

#include <esp_spiffs.h>
#include <esp_err.h>

namespace cima {

    system::Log Agent::LOGGER("Agent");

    std::string Agent::FLASH_FILESYSTEM_MOUNT_PATH = "/spiffs";

    std::string Agent::MESSAGE_TEMPLATE = "{\"greetings\":\"Hello %s!\", \"Temperature\":%f, \"Humidity\":%f}";

    Agent::Agent(iot::IoTHubManager &iotHubManager) 
        : iotHubManager(iotHubManager){}

    void Agent::welcome(std::string &visitorName){
        while(keepRunning){
            char greeting[128];
            sprintf(greeting, MESSAGE_TEMPLATE.c_str(), visitorName.c_str(),  0.0, 0.0);

            LOGGER.info(":-) Hello %s :-)", visitorName.c_str());
            if(iotHubManager.isReady()) {
                iotHubManager.sendMessage(greeting);
            }

            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
    }

    void Agent::cat(const std::string &filename){
        FILE* f = fopen(filename.c_str(), "r");
        if (f == NULL) {
            LOGGER.error("Failed to open %s", filename.c_str());
            return;
        }

        char buf[512];
        memset(buf, 0, sizeof(buf));
        auto readSize = fread(buf, 1, sizeof(buf), f);
        fclose(f);

        LOGGER.info("Read from %s:\n%.*s", filename.c_str(), readSize, buf);
    }

    int Agent::justPrint(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize){
        LOGGER.info("Payload :\n%.*s", size, payload);

        const char* responseMessage = "\"Thanks for asking\"";
        *responseSize = strlen(responseMessage) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return 200;
    }

    int Agent::whatIsTheTime(const unsigned char *payload, size_t size, unsigned char **response, size_t *responseSize){

        auto end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        const char * timeText = std::ctime(&end_time);

        char responseMessage[512];
        *responseSize =  sprintf(responseMessage, "{\"time\":\"%s\"}", timeText) + 1;
        *response = (unsigned char *)strdup(responseMessage);

        return 200;
    }

    bool Agent::mountFlashFileSystem(){
        esp_vfs_spiffs_conf_t conf = {
            .base_path = FLASH_FILESYSTEM_MOUNT_PATH.c_str(),
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = false
        };

        esp_err_t ret = esp_vfs_spiffs_register(&conf);

        if (ret != ESP_OK) {
            if (ret == ESP_FAIL) {
                LOGGER.error("Failed to mount or format filesystem");
            } else if (ret == ESP_ERR_NOT_FOUND) {
                LOGGER.error("Failed to find SPIFFS partition");
            } else {
                LOGGER.error("Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
            }
            return false;
        }

        size_t total = 0, used = 0;
        ret = esp_spiffs_info(NULL, &total, &used);
        if (ret != ESP_OK) {
            LOGGER.error("Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        } else {
            LOGGER.error("Partition size: total: %d, used: %d", total, used);
        }

        return true;
    }

    void Agent::setupNetwork(system::WifiManager &wifiManager){
        LOGGER.info(" > WiFi");
        auto credentials = std::move(readWifiCredentials());
        wifiManager.resetNetwork(credentials.first, credentials.second);

        wifiManager.start();
    }

    std::pair<std::string, std::string> Agent::readWifiCredentials(){
        std::ifstream in(FLASH_FILESYSTEM_MOUNT_PATH + "/connectivity/wifi.json");
        std::string wifiJson((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

        cJSON *root = cJSON_Parse(wifiJson.c_str());
        cJSON *networks = cJSON_GetObjectItem(root, "Networks");
        for (int i = 0 ; i < cJSON_GetArraySize(networks) ; i++){

            cJSON *network = cJSON_GetArrayItem(networks, i);
            auto ssid = cJSON_GetObjectItem(network, "SSID")->valuestring;
            auto passphrase = cJSON_GetObjectItem(network, "password")->valuestring;

            //TODO for now we just support connectivity to one network or bust
            return std::pair<std::string, std::string>(std::string(ssid), std::string(passphrase));
        }

        //TODO exception that we have not networks specified
        LOGGER.error("No WI-fi networks specified.");
        return std::pair<std::string, std::string>("internet", "");
    }

    void Agent::mainLoop(){
        while(keepRunning){
            for(auto function : mainLoopFunctions){
                function();
            }

            //TODO safety slowdown to avoid starvation - to be removed once finalized whole concept
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    void Agent::registerToMainLoop(std::function<void()> function){
        mainLoopFunctions.push_back(function);
    }

    void Agent::stop(){
        keepRunning = false;
    }
}