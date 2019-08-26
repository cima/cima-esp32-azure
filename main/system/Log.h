#pragma once

namespace cima::system {
    class Log {

        static bool started;

        std::string &loggerName;

    public:

        Log(std::string &name) : loggerName(name) {};

        void init();
        void info(const char* format, ...);
        void error(const char* format, ...);
    };
}