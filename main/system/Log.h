#pragma once

#include <string>

namespace cima::system {
    class Log {

        static bool started;

        const std::string &loggerName;

    public:

        Log(const std::string &name) : loggerName(name) {};

        void init();
        void debug(const char* format, ...) const;
        void info(const char* format, ...) const;
        void error(const char* format, ...) const;
    };
}