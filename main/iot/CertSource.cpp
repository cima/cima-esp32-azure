#include "CertSource.h"

#include <string>
#include <cstring>
#include <utility>
#include <memory>
#include <fstream>

namespace cima::iot {

    CertSource::CertSource(const std::string &privateKeyFile, const std::string &certificateFile) :
        pemPrivateKey(privateKeyFile), pemCertificate(certificateFile) {}

    const std::string CertSource::getPemPrivateKey() const {
        std::ifstream in(pemPrivateKey, std::ios_base::binary);
        std::string privateKey((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));

        return privateKey;
    }

    const std::string CertSource::getPemCertificate() const {
        std::ifstream in(pemCertificate, std::ios_base::binary);
        std::string certificate((std::istreambuf_iterator<char>(in)), (std::istreambuf_iterator<char>()));

        return certificate;
    }
}