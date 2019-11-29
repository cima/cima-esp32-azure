#pragma once

#include <stdlib.h>
#include <string>

namespace cima::iot {

    class CertSource {
        std::string pemPrivateKey;
        std::string pemCertificate;

    public:
        CertSource();

        const std::string &getPemPrivateKey() const;

        const std::string &getPemCertificate() const;
    };
}