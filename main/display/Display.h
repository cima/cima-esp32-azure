#pragma once

#include <list>

#include <iot_ssd1306.h>
#include <ssd1306_fonts.h>

#include "../system/WireManager.h"
#include "../system/Log.h"
#include "StatusIcon.h"

namespace cima::display{
    class Display {

        static const ::cima::system::Log LOG;

    public:
        static const ssd1306_config_t LILYGO_OLED_CONFIG;

    private:

        ::cima::system::WireManager &wire;

        CSsd1306 oledDisplay;
        std::list<StatusIcon *> statusIcons;
    public:
        Display(::cima::system::WireManager &wire, const ssd1306_config_t config);
        esp_err_t showTemperature(float temprature, float humidity, float pressure);
        void addStatusIcon(StatusIcon *statusIcon);

    private:
        void initDisplay();
        void drawStatus();
    };

}