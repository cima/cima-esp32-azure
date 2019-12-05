#pragma once

#include <stdlib.h>
#include <string>

#include "../system/Log.h"

namespace cima::iot {

    class CertSource {
        const std::string pemPrivateKey;
        const std::string pemCertificate;

        static const ::cima::system::Log LOG;

    public:
        CertSource(const std::string &privateKeyFile, const std::string &certificateFile);

        const std::string getPemPrivateKey() const;

        const std::string getPemCertificate() const;

        const std::string getCommonName() const;
    };
}