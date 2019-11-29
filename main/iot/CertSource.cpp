#include "CertSource.h"
namespace cima::iot {

    CertSource::CertSource() {
        pemPrivateKey = "-----BEGIN PRIVATE KEY----- \
... PEM encoded key content here ... \
-----END PRIVATE KEY-----";

        pemCertificate = "-----BEGIN CERTIFICATE----- \
... PEM encoded certificate content here ... \
-----END CERTIFICATE-----";
    }


    const std::string &CertSource::getPemPrivateKey() const {
        return pemPrivateKey;
    }

    const std::string &CertSource::getPemCertificate() const {
        return pemCertificate;
    }
}