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


    std::string CertSource::getPemPrivateKey() {
        return pemPrivateKey;
    }

    std::string CertSource::getPemCertificate() {
        return pemCertificate;
    }
}