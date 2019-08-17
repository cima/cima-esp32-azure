#pragma once

#include <stdlib.h>
#include <string>

class CertSource {
    std::string pemPrivateKey;
    std::string pemCertificate;

public:
    CertSource();

    std::string getPemPrivateKey();

    std::string getPemCertificate();
};