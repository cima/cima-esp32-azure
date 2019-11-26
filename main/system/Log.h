#pragma once

#include <string>

namespace cima::system {
    class Log {

        static bool started;

        const std::string &loggerName;

    public:

        Log(const std::string &name) : loggerName(name) {};

        void init();
        void info(const char* format, ...);
        void error(const char* format, ...);
    };
}