#pragma once

#include <string>
#include <list>
#include <iterator>
#include <thread>

#include <esp_event_base.h>
#include <esp_modem.h>
#include <freertos/portmacro.h>
#include <boost/signals2/signal.hpp>
#include <boost/scoped_ptr.hpp>

#include "../Log.h"
#include "NetworkManager.h"

/*
// Global variables referenced by esp_modem.c
extern int CONFIG_EXAMPLE_UART_RX_BUFFER_SIZE;
extern int CONFIG_EXAMPLE_UART_TX_BUFFER_SIZE;
extern int CONFIG_EXAMPLE_UART_EVENT_QUEUE_SIZE;
extern int CONFIG_EXAMPLE_UART_PATTERN_QUEUE_SIZE;
extern uint32_t CONFIG_EXAMPLE_UART_EVENT_TASK_STACK_SIZE;
extern UBaseType_t CONFIG_EXAMPLE_UART_EVENT_TASK_PRIORITY;

extern int CONFIG_EXAMPLE_UART_MODEM_TX_PIN;
extern int CONFIG_EXAMPLE_UART_MODEM_RX_PIN;
extern int CONFIG_EXAMPLE_UART_MODEM_RTS_PIN;
extern int CONFIG_EXAMPLE_UART_MODEM_CTS_PIN;

extern const char *CONFIG_EXAMPLE_MODEM_APN;

extern const char *CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME;
extern const char *CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD;
*/

namespace cima::system::network {

class ModemManager : public NetworkManager {
    static const char *TAG;

    esp_modem_dte_config_t config;

    modem_dte_t *dte;
    modem_dce_t *dce;

    bool keepRunning = true;
    boost::scoped_ptr<std::thread> reconnectLoop;

public:
    ModemManager(const esp_modem_dte_config_t &config);

    void init();
    void stop();

    virtual bool isConnected();

private:
    bool connect();
    void disconnect();
    esp_err_t preparePppStructure(modem_dte_t *dte);
    void onStop();

    void modemEventHandler(esp_event_base_t event_base, int32_t event_id, void *event_data);
    static void modemEventHandlerStatic(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
};

}