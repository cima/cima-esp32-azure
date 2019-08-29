#pragma once

#include <stdlib.h>
#include <string>

namespace cima::iot {

    class CertSource {
        std::string pemPrivateKey;
        std::string pemCertificate;

    public:
        CertSource();

        std::string getPemPrivateKey();

        std::string getPemCertificate();
    };
}