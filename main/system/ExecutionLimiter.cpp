#include "ExecutionLimiter.h"

namespace cima::system {

    ExecutionLimiter::ExecutionLimiter(std::chrono::milliseconds onceInMilliseconds){
        this->onceInMilliseconds = onceInMilliseconds;
    }

    bool ExecutionLimiter::canExecute(){
        auto now = std::chrono::high_resolution_clock::now();
        auto sinceLastExecution = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecution);
        if(sinceLastExecution >= onceInMilliseconds){
            lastExecution = now;
            return true;
        }

        return false;
    }
}