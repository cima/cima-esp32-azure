#include "CertSource.h"

#include <string>
#include <cstring>
#include <utility>
#include <memory>
#include <fstream>

#include <mbedtls/oid.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>

namespace cima::iot {

    const ::cima::system::Log CertSource::LOG("CertSource");

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

    const std::string CertSource::getCommonName() const{
        mbedtls_x509_crt chain;
        mbedtls_x509_crt_init(&chain);        
        
        const std::string pemCert(std::move(getPemCertificate()));

        if(pemCert.empty()){
            return "";
        }

        int parseError = mbedtls_x509_crt_parse(&chain, (const unsigned char *)pemCert.c_str(), pemCert.size() + 1);

        if(parseError) {
            LOG.error("Parsing of certificate failed: %x", parseError);
            return "";
        }

        std::string result;

        for(auto name = &chain.subject; name != NULL; name = name->next )
        {
            if( MBEDTLS_OID_CMP( MBEDTLS_OID_AT_CN, &name->oid ) ) {                
                LOG.debug(" Miss: %.*s", name->val.len, name->val.p);
            }else {
                result = std::string((const char *)name->val.p, name->val.len);
                LOG.debug("Match: %.*s", name->val.len, name->val.p);
            }

            LOG.debug("  +- len %d",name->oid.len);
            if(name->oid.len == 3){
                LOG.debug("  +- oid %x.%x.%x.%x", name->oid.p[0] / 40, name->oid.p[0] % 40, name->oid.p[1], name->oid.p[2]);
            }
        }

        mbedtls_x509_crt_free(&chain);
        return result;
    }
}