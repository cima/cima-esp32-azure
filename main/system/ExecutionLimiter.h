#pragma once

#include <chrono>

namespace cima::system {

    class ExecutionLimiter {
        std::chrono::milliseconds onceInMilliseconds;
        std::chrono::time_point<std::chrono::high_resolution_clock> lastExecution;

    public:
        ExecutionLimiter(std::chrono::milliseconds onceInMilliseconds);
        bool canExecute();
    };
}
