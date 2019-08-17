#include "Log.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <HardwareSerial.h>

namespace cima {

    void Log::init() {
        Serial.begin(115200);
    }

    void Log::info(const char* format, ...) {
        char buffer [512];

        va_list args;
        va_start(args, format);
   
        vsnprintf(buffer, 511, format, args);

        Serial.println(buffer);
        va_end(args);   
    }

    void Log::error(const char* format, ...) {
        char buffer [512];

        va_list args;
        va_start(args, format);
   
        vsnprintf(buffer, 511, format, args);

        Serial.println(buffer);
        va_end(args);   
    }

}