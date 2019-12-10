#include "ButtonController.h"

namespace cima::system {
    
    const Log ButtonController::LOGGER("ButtonController");

    ButtonController::ButtonController(gpio_num_t buttonGpioPin) : buttonGpioPin(buttonGpioPin) {}

    void ButtonController::initButton(){
        gpio_config_t io_conf;
        io_conf.intr_type = GPIO_INTR_POSEDGE;
        io_conf.pin_bit_mask = 1ULL << buttonGpioPin;    
        io_conf.mode = GPIO_MODE_INPUT;
        io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

        esp_err_t gpioError = gpio_config(&io_conf);
        LOGGER.debug("Po globál dobrý 0x%x", gpioError);

        int isrFlags = ESP_INTR_FLAG_LEVEL1 | ESP_INTR_FLAG_SHARED;
        esp_err_t isrError = gpio_install_isr_service(isrFlags);
        if(isrError == ESP_OK){
            esp_err_t handlerError = gpio_isr_handler_add(buttonGpioPin, &ButtonController::gpioButtonHandlerWrapper, this);
            LOGGER.debug("Instalace handleru 0x%x", handlerError);
        }else{
            LOGGER.error("Can't instal ISR service: 0x%x", isrError);
        }

        LOGGER.debug("Button controller overall fine");
    }

    void ButtonController::handleClicks(){
        std::lock_guard<std::mutex> guard(clicksMutex);
        if(clicks.empty()){
            return;
        }
        auto eventTime = clicks.front();
        LOGGER.info("Handling button event from: %s", std::ctime(&eventTime));
        buttonSignal();
        clicks.pop();
    }

    void ButtonController::addHandler(std::function<void(void)> func){
        buttonSignal.connect(func);
    }

    void ButtonController::gpioButtonHandler(){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.
        auto nowChrono = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(nowChrono);
    
        std::lock_guard<std::mutex> guard(clicksMutex);
        clicks.push(nowTime);
    }

    void ButtonController::gpioButtonHandlerWrapper(void *arg){
        // ATTENTION: This is IRS call. No monkey bussiness. Just forward over some parralel primitives.
        ((ButtonController *)arg)->gpioButtonHandler();
    }
}