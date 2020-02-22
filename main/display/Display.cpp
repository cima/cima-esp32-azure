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

    Display::Display(::cima::system::WireManager &wire, const ssd1306_config_t config) 
        : wire(wire), oledDisplay(&wire.getWire(), config) {

        initDisplay();
        LOG.debug("Aspon konstruktor.");
    }

    void Display::initDisplay(){
        oledDisplay.clear_screen(0);
    }
    
    void Display::addStatusIcon(StatusIcon *statusIcon){
        statusIcons.push_back(statusIcon);
    }

    esp_err_t Display::showTemperature(float temprature, float humidity, float pressure) {

        drawStatus();

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

    void Display::drawStatus() {
        int index = 0;
        for(auto icon : statusIcons){
            if( ! icon){
                continue;
            }

            oledDisplay.draw_bitmap(index * 8, 0, icon->getIcon(), 8, 8);
            index++;
        }
    }
}