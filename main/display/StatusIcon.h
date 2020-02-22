#pragma once

#include "../system/Log.h"

namespace cima::display{
    class StatusIcon {

    protected:
        const uint8_t *icon;

    public:
        static const uint8_t ICON_WIFI_88[8];
        static const uint8_t ICON_AZURE_88[8];
        static const uint8_t BLANK[8];

        StatusIcon(const uint8_t icon[8]);

        virtual const uint8_t * getIcon() = 0;
    };

    class BooleanStatusIcon : StatusIcon {
        bool isActive = false;

    public:
        BooleanStatusIcon(const uint8_t icon[8]);
        virtual const uint8_t * getIcon();
        void setActive(bool active);
    };
}