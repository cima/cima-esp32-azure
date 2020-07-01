#include "ModemManager.h"

#include <esp_modem.h>
#include <esp_log.h> //TODO use my logging

#include "M590.h"
/*
// Global variables referenced by esp_modem.c
int CONFIG_EXAMPLE_UART_RX_BUFFER_SIZE;
int CONFIG_EXAMPLE_UART_TX_BUFFER_SIZE;
int CONFIG_EXAMPLE_UART_EVENT_QUEUE_SIZE;
int CONFIG_EXAMPLE_UART_PATTERN_QUEUE_SIZE;
uint32_t CONFIG_EXAMPLE_UART_EVENT_TASK_STACK_SIZE;
UBaseType_t CONFIG_EXAMPLE_UART_EVENT_TASK_PRIORITY;

int CONFIG_EXAMPLE_UART_MODEM_TX_PIN;
int CONFIG_EXAMPLE_UART_MODEM_RX_PIN;
int CONFIG_EXAMPLE_UART_MODEM_RTS_PIN;
int CONFIG_EXAMPLE_UART_MODEM_CTS_PIN;

const char *CONFIG_EXAMPLE_MODEM_APN;

const char *CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME;
const char *CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD;
*/

namespace cima::system::network {

    const char *ModemManager::TAG = "ModemManager";

    ModemManager::ModemManager(const esp_modem_dte_config_t &config) {
        this->config = config;
    }

    void ModemManager::init() {
        tcpip_adapter_init(); //FIXME this must be called once in a runtime
        
        /* create dte object -- one time action, it is so so tangled in global shit*/
        dte = esp_modem_dte_init(&config);

        reconnectLoop.reset(new std::thread([&](){
            while(keepRunning){
                auto success = connect();
                if( ! success) {
                    break;
                }
                do{
                    std::this_thread::sleep_for(std::chrono::seconds(30));
                } while(keepRunning && success && connected);
            }
            onStop();
        }));
    }

    bool ModemManager::connect() {

        ESP_ERROR_CHECK(esp_prepare_ppp_structure(dte));
        /* Register event handler */
        ESP_ERROR_CHECK(esp_modem_add_event_handler(dte, ModemManager::modemEventHandlerStatic, this));

        /* create dce object */
        dce = m590_init(dte);
        if( ! dce){
            ESP_LOGE(TAG, "Initialization of M590 failed.");
            return false;
        }

        ESP_ERROR_CHECK(dce->set_flow_ctrl(dce, config.flow_control));
        ESP_ERROR_CHECK(dce->store_profile(dce));
        /* Print Module ID, Operator, IMEI, IMSI */
        ESP_LOGI(TAG, "Module: %s", dce->name);
        ESP_LOGI(TAG, "Operator: %s", dce->oper);
        ESP_LOGI(TAG, "IMEI: %s", dce->imei);
        ESP_LOGI(TAG, "IMSI: %s", dce->imsi);
        /* Get signal quality */
        uint32_t rssi = 0, ber = 0;
        ESP_ERROR_CHECK(dce->get_signal_quality(dce, &rssi, &ber));
        ESP_LOGI(TAG, "rssi: %d, ber: %d", rssi, ber);

        /* Get battery voltage */
        /*
        uint32_t voltage = 0, bcs = 0, bcl = 0;
        ESP_ERROR_CHECK(dce->get_battery_status(dce, &bcs, &bcl, &voltage));
        ESP_LOGI(TAG, "Battery voltage: %d mV", voltage);
        */

        /* Setup PPP environment */
        //ESP_ERROR_CHECK(); //TODO tohle je na hovno
        auto result = esp_modem_setup_ppp(dte);

        return result == ESP_OK;
    }

    void ModemManager::disconnect() {
        /* Exit PPP mode */
        ESP_ERROR_CHECK(esp_modem_exit_ppp(dte));
    }

    void ModemManager::stop() {
        keepRunning = false;
    }

    bool ModemManager::isConnected() {
        return connected;
    }

    void ModemManager::onStop() {

        disconnect();

        /* Power down module */
        ESP_ERROR_CHECK(dce->power_down(dce));
        ESP_LOGI(TAG, "Power down");
        ESP_ERROR_CHECK(dce->deinit(dce));
        ESP_ERROR_CHECK(dte->deinit(dte));
    }

    void ModemManager::modemEventHandler(esp_event_base_t event_base, int32_t event_id, void *event_data) {
        switch (event_id) {
            case MODEM_EVENT_PPP_START:
                ESP_LOGI(TAG, "Modem PPP Started");
                break;
            case MODEM_EVENT_PPP_CONNECT:
            {
                ESP_LOGI(TAG, "Modem Connect to PPP Server");
                ppp_client_ip_info_t *ipinfo = (ppp_client_ip_info_t *)(event_data);
                ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
                ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&ipinfo->ip));
                ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&ipinfo->netmask));
                ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&ipinfo->gw));
                ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&ipinfo->ns1));
                ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&ipinfo->ns2));
                ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
                connected = true;
                auto signalThread = std::thread([&](){networkUpSignal();});
                signalThread.join();
            }
                break;
            case MODEM_EVENT_PPP_DISCONNECT:
                ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
                connected = false;
                break;
            case MODEM_EVENT_PPP_STOP:
            {
                ESP_LOGI(TAG, "Modem PPP Stopped");
                connected = false;
                auto signalThread = std::thread([&](){networkDownSignal();});
                signalThread.join();
                onStop();
            }
                break;
            case MODEM_EVENT_UNKNOWN:
                ESP_LOGW(TAG, "Unknow line received: %s", (char *)event_data);
                break;
            default:
                break;
        }
    }

    void ModemManager::modemEventHandlerStatic(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data){
        ((ModemManager *)event_handler_arg)->modemEventHandler(event_base, event_id, event_data);
    }
}