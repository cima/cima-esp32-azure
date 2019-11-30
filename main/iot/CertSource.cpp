#include "CertSource.h"
namespace cima::iot {

    CertSource::CertSource() :
        pemPrivateKey("-----BEGIN RSA PRIVATE KEY-----\r\n\
... PEM encoded key content here ...\r\n\
-----END RSA PRIVATE KEY-----\r\n"),

        pemCertificate("-----BEGIN CERTIFICATE-----\r\n\
... PEM encoded certificate content here ...\r\n\
-----END CERTIFICATE-----\r\n") {}

    const std::string &CertSource::getPemPrivateKey() const {
        return pemPrivateKey;
    }

    const std::string &CertSource::getPemCertificate() const {
        return pemCertificate;
    }
}