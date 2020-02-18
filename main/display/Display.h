#pragma once

#include <iot_ssd1306.h>
#include <ssd1306_fonts.h>

#include "../system/WireManager.h"
#include "../system/Log.h"

namespace cima::display{
    class Display {

        static const ::cima::system::Log LOG;

    public:
        static const ssd1306_config_t LILYGO_OLED_CONFIG;

    public:
        static const uint8_t ICON_WIFI_88[8];
        static const uint8_t ICON_AZURE_88[8];

    private:

        ::cima::system::WireManager &wire;

        CSsd1306 oledDisplay;
    public:
        Display(::cima::system::WireManager &wire, const ssd1306_config_t config);
        esp_err_t showTemperature(float temprature, float humidity, float pressure, bool isWiFi, bool isAzure);

    private:
        void initDisplay();
        void drawStatus(bool isWiFi, bool isAzure);
    };

}