#include "StatusIcon.h"

namespace cima::display{

    const uint8_t StatusIcon::ICON_WIFI_88[8] = {
        0b00000000,
        0b00011000,
        0b01111110,
        0b10000001,
        0b00111100,
        0b01000010,
        0b00011000,
        0b00011000
    };

    const uint8_t StatusIcon::ICON_AZURE_88[8] = {
        0b00011000,
        0b00110100,
        0b01100110,
        0b01100110,
        0b11100111,
        0b11000011,
        0b11001111,
        0b00111111
    };

    const uint8_t StatusIcon::BLANK[8] = {
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000,
        0b00000000
    };

    StatusIcon::StatusIcon(const uint8_t icon[8]) {
        this->icon = icon;
    }

    BooleanStatusIcon::BooleanStatusIcon(const uint8_t icon[8]) : StatusIcon(icon) {}

    const uint8_t *BooleanStatusIcon::getIcon(){
        if(isActive){
            return this->icon;
        }
        return StatusIcon::BLANK;
    }

    void BooleanStatusIcon::setActive(bool active){
        this->isActive = active;
    }
}