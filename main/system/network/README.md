# Network
...
## Modem

![alt text](https://upload.wikimedia.org/wikipedia/commons/thumb/2/2a/EIA_232_DTE_DCE_DCE_DTE.png/500px-EIA_232_DTE_DCE_DCE_DTE.png "Modem network topology")
_[Data circuit-terminating equipment](https://en.wikipedia.org/wiki/Data_circuit-terminating_equipment)_

- _DCE data circuit equipement_: An actual Modem -- and interface betwen carrier physical language and a computer physical language
- _DTE data teminal equipment_: A computer

**NOTE:** ESP-IDF introduces two common GSM modems (BG96, SIM800L). However I have M590E which needs to be introduced.

### Neoway M590E

(https://cdn.instructables.com/ORIG/FX6/UAHQ/IZ6CYGIU/FX6UAHQIZ6CYGIU.pdf)

### PPPoS

Example at [PPP over Serial (PPPoS) client example](https://github.com/espressif/esp-idf/tree/master/examples/protocols/pppos_client) Introduces PPPoS modem functionality and shows how to utilize it, but references available GSM modems (BG96, SIM800L). Introduciton of new modem is inevitable.

#### Constants

- *CONFIG_EXAMPLE_UART_RX_BUFFER_SIZE* (can be variable)
- *CONFIG_EXAMPLE_UART_TX_BUFFER_SIZE* (can be variable)

- *CONFIG_EXAMPLE_UART_PATTERN_QUEUE_SIZE* (can be variable)

- *CONFIG_EXAMPLE_UART_EVENT_QUEUE_SIZE* (can be variable)
- *CONFIG_EXAMPLE_UART_EVENT_TASK_STACK_SIZE* (can be variable)
- *CONFIG_EXAMPLE_UART_EVENT_TASK_PRIORITY* (can be variable)

- *CONFIG_EXAMPLE_UART_MODEM_TX_PIN* (can be variable)
- *CONFIG_EXAMPLE_UART_MODEM_RX_PIN* (can be variable)
- *CONFIG_EXAMPLE_UART_MODEM_RTS_PIN* (can be variable)
- *CONFIG_EXAMPLE_UART_MODEM_CTS_PIN* (can be variable)

- *CONFIG_EXAMPLE_MODEM_APN* (can be variable)

- *CONFIG_EXAMPLE_MODEM_PPP_AUTH_USERNAME* (can be variable)
- *CONFIG_EXAMPLE_MODEM_PPP_AUTH_PASSWORD* (can be variable)
