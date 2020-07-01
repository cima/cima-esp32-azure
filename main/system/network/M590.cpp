#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include <esp_modem_dce_service.h>
#include "M590.h"

#define MODEM_RESULT_CODE_POWERDOWN "OK"

#define MODEM_NETWORK_STATUS_REGISTERED 1
#define MODEM_NETWORK_STATUS_REFUSED 3
#define MODEM_NETWORK_STATUS_REGISTERED_ROAMING 5

#define MODEM_COMMAND_NETWORK_REGISTRSTION "CREG"

/**
 * @brief Macro defined for error checking
 *
 */
static const char *DCE_TAG = "M590";
#define DCE_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                \
    {                                                                                 \
        if (!(a))                                                                     \
        {                                                                             \
            ESP_LOGE(DCE_TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                            \
        }                                                                             \
    } while (0)

/**
 * @brief M590 Modem
 *
 */
typedef struct {
    void *priv_resource; /*!< Private resource */
    modem_dce_t parent;  /*!< DCE parent class */
} m590_modem_dce_t;


static int networkRegState;

/**
 * @brief Handle response from AT+CSQ
 */
static esp_err_t m590_handle_csq(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    m590_modem_dce_t *m590_dce = __containerof(dce, m590_modem_dce_t, parent);
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else if (!strncmp(line, "+CSQ", strlen("+CSQ"))) {
        /* store value of rssi and ber */
        uint32_t **csq = (uint32_t **)m590_dce->priv_resource;
        /* +CSQ: <rssi>,<ber> */
        sscanf(line, "%*s%d,%d", csq[0], csq[1]);
        err = ESP_OK;
    }
    return err;
}

/**
 * @brief Handle response from AT+CBC
 */
static esp_err_t m590_handle_cbc(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    m590_modem_dce_t *m590_dce = __containerof(dce, m590_modem_dce_t, parent);
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else if (!strncmp(line, "+CBC", strlen("+CBC"))) {
        /* store value of bcs, bcl, voltage */
        uint32_t **cbc = (uint32_t **)m590_dce->priv_resource;
        /* +CBC: <bcs>,<bcl>,<voltage> */
        sscanf(line, "%*s%d,%d,%d", cbc[0], cbc[1], cbc[2]);
        err = ESP_OK;
    }
    return err;
}

/**
 * @brief Handle response from +++
 */
static esp_err_t m590_handle_exit_data_mode(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_NO_CARRIER)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    }
    return err;
}

/**
 * @brief Handle response from ATD*99#
 */
static esp_err_t m590_handle_atd_ppp(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_CONNECT)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    }
    return err;
}

/**
 * @brief Handle response from AT+CGMM
 */
static esp_err_t m590_handle_modem_startup(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, "MODEM:STARTUP")) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    }
    return err;
}

/**
 * @brief Handle response from AT+CGMM
 */
static esp_err_t m590_handle_cgmm(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else {
        int len = snprintf(dce->name, MODEM_MAX_NAME_LENGTH, "%s", line);
        if (len > 2) {
            /* Strip "\r\n" */
            strip_cr_lf_tail(dce->name, len);
            err = ESP_OK;
        }
    }
    return err;
}

/**
 * @brief Handle response from AT+CGSN
 */
static esp_err_t m590_handle_cgsn(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else {
        int len = snprintf(dce->imei, MODEM_IMEI_LENGTH + 1, "%s", line);
        if (len > 2) {
            /* Strip "\r\n" */
            strip_cr_lf_tail(dce->imei, len);
            err = ESP_OK;
        }
    }
    return err;
}

/**
 * @brief Handle response from AT+CIMI
 */
static esp_err_t m590_handle_cimi(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else {
        int len = snprintf(dce->imsi, MODEM_IMSI_LENGTH + 1, "%s", line);
        if (len > 2) {
            /* Strip "\r\n" */
            strip_cr_lf_tail(dce->imsi, len);
            err = ESP_OK;
        }
    }
    return err;
}

static bool is_m590_gibberish(const char *line)
{
    return strstr(line, "+PBREADY") != nullptr;
}

static bool is_m590_crash(const char *line)
{
    return strstr(line, "MODEM:STARTUP") != nullptr;
}

/**
 * @brief Handle response from AT+CREG?
 */
static esp_err_t m590_handle_creg(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (is_m590_gibberish(line)){
        err = ESP_OK;
    } else if (is_m590_crash(line)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
        networkRegState = -1;
    } else if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else if (!strncmp(line, "+" MODEM_COMMAND_NETWORK_REGISTRSTION ": ", strlen("+" MODEM_COMMAND_NETWORK_REGISTRSTION ": "))) {
        /* there might be some random spaces in operator's name, we can not use sscanf to parse the result */
        /* strtok will break the string, we need to create a copy */
        size_t len = strlen(line);
        char *line_copy = (char *)malloc(len + 1);
        strcpy(line_copy, line);
        /* +COPS: <mode>[, <format>[, <oper>]] */
        char *str_ptr = NULL;
        char *p[3];
        uint8_t i = 0;
        /* strtok will broke string by replacing delimiter with '\0' */
        p[i] = strtok_r(line_copy, ",", &str_ptr);
        while (p[i]) {
            p[++i] = strtok_r(NULL, ",", &str_ptr);
        }
        if (i >= 2) {
            networkRegState = atoi(p[1]); //FIXME zase global
            err = ESP_OK;
        }
        free(line_copy);
    }
    return err;
}

/**
 * @brief Handle response from AT+COPS?
 */
static esp_err_t m590_handle_cops(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_SUCCESS)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    } else if (strstr(line, MODEM_RESULT_CODE_ERROR)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_FAIL);
    } else if (!strncmp(line, "+COPS", strlen("+COPS"))) {
        /* there might be some random spaces in operator's name, we can not use sscanf to parse the result */
        /* strtok will break the string, we need to create a copy */
        size_t len = strlen(line);
        char *line_copy = (char *)malloc(len + 1);
        strcpy(line_copy, line);
        /* +COPS: <mode>[, <format>[, <oper>]] */
        char *str_ptr = NULL;
        char *p[3];
        uint8_t i = 0;
        /* strtok will broke string by replacing delimiter with '\0' */
        p[i] = strtok_r(line_copy, ",", &str_ptr);
        while (p[i]) {
            p[++i] = strtok_r(NULL, ",", &str_ptr);
        }
        if (i >= 3) {
            int len = snprintf(dce->oper, MODEM_MAX_OPERATOR_LENGTH, "%s", p[2]);
            if (len > 2) {
                /* Strip "\r\n" */
                strip_cr_lf_tail(dce->oper, len);
                err = ESP_OK;
            }
        }
        free(line_copy);
    }
    return err;
}

/**
 * @brief Handle response from AT+CPWROFF 
 */
static esp_err_t m590_handle_power_down(modem_dce_t *dce, const char *line)
{
    esp_err_t err = ESP_FAIL;
    if (strstr(line, MODEM_RESULT_CODE_POWERDOWN)) {
        err = esp_modem_process_command_done(dce, MODEM_STATE_SUCCESS);
    }
    return err;
}

esp_err_t esp_modem_dce_set_flow_ctrl_noop(modem_dce_t *dce, modem_flow_ctrl_t flow_ctrl)
{
    return ESP_OK;
}

/**
 * @brief Get signal quality
 *
 * @param dce Modem DCE object
 * @param rssi received signal strength indication
 * @param ber bit error ratio
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_signal_quality(modem_dce_t *dce, uint32_t *rssi, uint32_t *ber)
{
    modem_dte_t *dte = dce->dte;
    m590_modem_dce_t *m590_dce = __containerof(dce, m590_modem_dce_t, parent);
    uint32_t *resource[2] = {rssi, ber};
    m590_dce->priv_resource = resource;
    dce->handle_line = m590_handle_csq;
    DCE_CHECK(dte->send_cmd(dte, "AT+CSQ\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "inquire signal quality failed", err);
    ESP_LOGD(DCE_TAG, "inquire signal quality ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get battery status
 *
 * @param dce Modem DCE object
 * @param bcs Battery charge status
 * @param bcl Battery connection level
 * @param voltage Battery voltage
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_battery_status(modem_dce_t *dce, uint32_t *bcs, uint32_t *bcl, uint32_t *voltage)
{
    modem_dte_t *dte = dce->dte;
    m590_modem_dce_t *m590_dce = __containerof(dce, m590_modem_dce_t, parent);
    uint32_t *resource[3] = {bcs, bcl, voltage};
    m590_dce->priv_resource = resource;
    dce->handle_line = m590_handle_cbc;
    DCE_CHECK(dte->send_cmd(dte, "AT+CBC\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "inquire battery status failed", err);
    ESP_LOGD(DCE_TAG, "inquire battery status ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Set Working Mode
 *
 * @param dce Modem DCE object
 * @param mode woking mode
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_set_working_mode(modem_dce_t *dce, modem_mode_t mode)
{
    modem_dte_t *dte = dce->dte;
    switch (mode) {
    case MODEM_COMMAND_MODE:
        dce->handle_line = m590_handle_exit_data_mode;
        DCE_CHECK(dte->send_cmd(dte, "+++", MODEM_COMMAND_TIMEOUT_MODE_CHANGE) == ESP_OK, "send command failed", err);
        DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "enter command mode failed", err);
        ESP_LOGD(DCE_TAG, "enter command mode ok");
        dce->mode = MODEM_COMMAND_MODE;
        break;
    case MODEM_PPP_MODE:
        dce->handle_line = m590_handle_atd_ppp;
        DCE_CHECK(dte->send_cmd(dte, "ATD*99#\r", MODEM_COMMAND_TIMEOUT_MODE_CHANGE) == ESP_OK, "send command failed", err);
        DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "enter ppp mode failed", err);
        ESP_LOGD(DCE_TAG, "enter ppp mode ok");
        dce->mode = MODEM_PPP_MODE;
        break;
    default:
        ESP_LOGW(DCE_TAG, "unsupported working mode: %d", mode);
        goto err;
        break;
    }
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Power down
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_power_down(modem_dce_t *dce)
{
    modem_dte_t *dte = dce->dte;
    dce->handle_line = m590_handle_power_down;

    DCE_CHECK(dte->send_cmd(dte, "AT+CPWROFF\r", MODEM_COMMAND_TIMEOUT_POWEROFF) == ESP_OK, "send command failed", err);
    DCE_CHECK(dce->state == MODEM_STATE_SUCCESS, "power down failed", err);
    ESP_LOGD(DCE_TAG, "power down ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

static esp_err_t m590_wait_for_startup(m590_modem_dce_t *m590_dce)
{
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_modem_startup;
    DCE_CHECK(dte->send_cmd(dte, "\r", 15000) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get module name failed", err);
    ESP_LOGD(DCE_TAG, "Waiting for startup OK");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get DCE module name
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_module_name(m590_modem_dce_t *m590_dce)
{
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_cgmm;
    DCE_CHECK(dte->send_cmd(dte, "AT+CGMM\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get module name failed", err);
    ESP_LOGD(DCE_TAG, "get module name ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get DCE module IMEI number
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_imei_number(m590_modem_dce_t *m590_dce)
{
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_cgsn;
    DCE_CHECK(dte->send_cmd(dte, "AT+CGSN\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get imei number failed", err);
    ESP_LOGD(DCE_TAG, "get imei number ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get DCE module IMSI number
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_imsi_number(m590_modem_dce_t *m590_dce)
{
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_cimi;
    DCE_CHECK(dte->send_cmd(dte, "AT+CIMI\r", MODEM_COMMAND_TIMEOUT_DEFAULT) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get imsi number failed", err);
    ESP_LOGD(DCE_TAG, "get imsi number ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get network registration status
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_network_registration_status(m590_modem_dce_t *m590_dce)
{
    //CIMA
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_creg;
    DCE_CHECK(dte->send_cmd(dte, "AT+" MODEM_COMMAND_NETWORK_REGISTRSTION "?\r", MODEM_COMMAND_TIMEOUT_OPERATOR) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get network registration status failed", err);
    ESP_LOGD(DCE_TAG, "get network registration status ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Get Operator's name
 *
 * @param m590_dce m590 object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on error
 */
static esp_err_t m590_get_operator_name(m590_modem_dce_t *m590_dce)
{
    modem_dte_t *dte = m590_dce->parent.dte;
    m590_dce->parent.handle_line = m590_handle_cops;
    DCE_CHECK(dte->send_cmd(dte, "AT+COPS?\r", MODEM_COMMAND_TIMEOUT_OPERATOR) == ESP_OK, "send command failed", err);
    DCE_CHECK(m590_dce->parent.state == MODEM_STATE_SUCCESS, "get network operator failed", err);
    ESP_LOGD(DCE_TAG, "get network operator ok");
    return ESP_OK;
err:
    return ESP_FAIL;
}

/**
 * @brief Deinitialize M590 object
 *
 * @param dce Modem DCE object
 * @return esp_err_t
 *      - ESP_OK on success
 *      - ESP_FAIL on fail
 */
static esp_err_t m590_deinit(modem_dce_t *dce)
{
    m590_modem_dce_t *m590_dce = __containerof(dce, m590_modem_dce_t, parent);
    if (dce->dte) {
        dce->dte->dce = NULL;
    }
    free(m590_dce);
    return ESP_OK;
}

modem_dce_t *m590_init(modem_dte_t *dte)
{

    int countDown = 100; //FIXME FUJ fuj fuj
    const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
    /* malloc memory for m590_dce object */
    m590_modem_dce_t *m590_dce = (m590_modem_dce_t *)calloc(1, sizeof(m590_modem_dce_t));
    DCE_CHECK(m590_dce, "calloc m590_dce failed", err);

    DCE_CHECK(dte, "DCE should bind with a DTE", err_io);

    /* Bind DTE with DCE */
    m590_dce->parent.dte = dte;
    dte->dce = &(m590_dce->parent);
    /* Bind methods */
    m590_dce->parent.handle_line = NULL;
    m590_dce->parent.sync = esp_modem_dce_sync;
    m590_dce->parent.echo_mode = esp_modem_dce_echo;
    m590_dce->parent.store_profile = esp_modem_dce_store_profile;
    m590_dce->parent.set_flow_ctrl = esp_modem_dce_set_flow_ctrl_noop; //FIXME nemáme
    m590_dce->parent.define_pdp_context = esp_modem_dce_define_pdp_context;
    m590_dce->parent.hang_up = esp_modem_dce_hang_up;
    m590_dce->parent.get_signal_quality = m590_get_signal_quality; //Done
    m590_dce->parent.get_battery_status = m590_get_battery_status; //Done - but modem probably doesn't support this command
    m590_dce->parent.set_working_mode = m590_set_working_mode; //Done
    m590_dce->parent.power_down = m590_power_down;//Done
    m590_dce->parent.deinit = m590_deinit; // Done

    //TODO if modem is in autobaud rate this will never occur
    //DCE_CHECK(m590_wait_for_startup(m590_dce) == ESP_OK, "Waiting for startup message failed", err_io); //Done

    /* Sync between DTE and DCE */
    DCE_CHECK(esp_modem_dce_sync(&(m590_dce->parent)) == ESP_OK, "sync failed", err_io);
    /* Close echo */
    DCE_CHECK(esp_modem_dce_echo(&(m590_dce->parent), false) == ESP_OK, "close echo mode failed", err_io);
    /* Get Module name */
    DCE_CHECK(m590_get_module_name(m590_dce) == ESP_OK, "get module name failed", err_io); //Done
    /* Get IMEI number */
    DCE_CHECK(m590_get_imei_number(m590_dce) == ESP_OK, "get imei failed", err_io); //Done
    /* Get IMSI number */
    DCE_CHECK(m590_get_imsi_number(m590_dce) == ESP_OK, "get imsi failed", err_io); //Done

    //Wait for network
    networkRegState = 0;
    while(countDown-- > 0 && (networkRegState != MODEM_NETWORK_STATUS_REGISTERED && networkRegState != MODEM_NETWORK_STATUS_REGISTERED_ROAMING)){
        DCE_CHECK(m590_get_network_registration_status(m590_dce) == ESP_OK, "get network registration status", dirty_break); //Done
        ESP_LOGI(DCE_TAG, "Registration status: %d", networkRegState);
        
        // Explicit network registration refusal no need to try 
        if(networkRegState == MODEM_NETWORK_STATUS_REFUSED){
            ESP_LOGE(DCE_TAG, "Registration to GSM network refused");
        }
        vTaskDelay( xDelay );
        continue;
dirty_break:
        break;
    }

    if(networkRegState != MODEM_NETWORK_STATUS_REGISTERED && networkRegState != MODEM_NETWORK_STATUS_REGISTERED_ROAMING){
        ESP_LOGE(DCE_TAG, "Network registration failed. Last status %d", networkRegState);
        goto err_io;
    }

    /* Get operator name */
    DCE_CHECK(m590_get_operator_name(m590_dce) == ESP_OK, "get operator name failed", err_io); //Done
    return &(m590_dce->parent);
err_io:
    free(m590_dce);
err:
    return NULL;
}
