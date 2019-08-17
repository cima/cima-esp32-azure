#pragma once

namespace cima {
class Log {

public:
    void init();
    void info(const char* format, ...);
    void error(const char* format, ...);
};
}