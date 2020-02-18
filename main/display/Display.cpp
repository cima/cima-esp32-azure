#include "Display.h"

#include <thread>
#include <chrono>

#include <driver/gpio.h>

namespace cima::display{

    const ::cima::system::Log Display::LOG("Display");

    const ssd1306_config_t Display::LILYGO_OLED_CONFIG = {
        PERIPHS_IO_MUX_GPIO23_U, FUNC_GPIO23_GPIO23, 23, // CS Chip select
        PERIPHS_IO_MUX_GPIO16_U, FUNC_GPIO16_GPIO16, 16, // RES reset
        PERIPHS_IO_MUX_GPIO5_U,  FUNC_GPIO5_GPIO5,    5, // DC Data command
        PERIPHS_IO_MUX_GPIO4_U,  FUNC_GPIO4_GPIO4,    4, // DIN data input
        PERIPHS_IO_MUX_MTDO_U,   FUNC_MTDO_GPIO15,   15, // CLK clock
        0x00, // Lower column address
        0x10  // Higher column address
    };

    const uint8_t Display::ICON_WIFI_88[8] = {
        0b00000000,
        0b00011000,
        0b01111110,
        0b10000001,
        0b00111100,
        0b01000010,
        0b00011000,
        0b00011000
    };

    const uint8_t Display::ICON_AZURE_88[8] = {
        0b00011000,
        0b00110100,
        0b01100110,
        0b01100110,
        0b11100111,
        0b11000011,
        0b11001111,
        0b00111111
    };

    Display::Display(::cima::system::WireManager &wire, const ssd1306_config_t config) 
        : wire(wire), oledDisplay(&wire.getWire(), config) {
        
        initDisplay();



        LOG.debug("Aspon konstruktor.");
    }

    void Display::initDisplay(){
        oledDisplay.clear_screen(0);
    }
    
    esp_err_t Display::showTemperature(float temprature, float humidity, float pressure, bool isWiFi, bool isAzure) {

        drawStatus(isWiFi, isAzure);

        char tempraturestr[6];
        sprintf(tempraturestr, "%4.1f", temprature);
        tempraturestr[4] = '\0';

        oledDisplay.draw_string(0, 16, (const uint8_t *) "TEM:", 16, 1);
        oledDisplay.draw_3216char(36, 16, tempraturestr[0]);
        oledDisplay.draw_3216char(52, 16, tempraturestr[1]);
        oledDisplay.draw_char(70, 30, tempraturestr[2], 16, 1);
        oledDisplay.draw_3216char(75, 16, tempraturestr[3]);

        char humidityStr[9];
        sprintf(humidityStr, "%5.1f %%", humidity);

        oledDisplay.draw_string(0, 52, (const uint8_t *) "RH", 12, 0);
        oledDisplay.draw_string(12, 52, (const uint8_t *) humidityStr, 12, 1);

        char pressureStr[11];
        sprintf(pressureStr, "%6.1f hPa", pressure);

        oledDisplay.draw_string(12 + 8*6, 52, (const uint8_t *) "P", 12, 0);
        oledDisplay.draw_string(12 + 8*6 + 6, 52, (const uint8_t *) pressureStr, 12, 1);

        return oledDisplay.refresh_gram();
    }

    void Display::drawStatus(bool isWiFi, bool isAzure) {
        if(isWiFi){
            oledDisplay.draw_bitmap(0, 0, ICON_WIFI_88, 8, 8);
        }

        if(isAzure){
            oledDisplay.draw_bitmap(8, 0, ICON_AZURE_88, 8, 8);
        }
    }
}