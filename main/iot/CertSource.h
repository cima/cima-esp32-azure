#pragma once

#include <stdlib.h>
#include <string>

namespace cima::iot {

    class CertSource {
        const std::string pemPrivateKey;
        const std::string pemCertificate;

    public:
        CertSource(const std::string &privateKeyFile, const std::string &certificateFile);

        const std::string getPemPrivateKey() const;

        const std::string getPemCertificate() const;
    };
}