#pragma once

#include <exception>

namespace cima::system {
    class ManagerNotInitializedException : public std::exception {
    public:
        ManagerNotInitializedException() = default;
    };
}