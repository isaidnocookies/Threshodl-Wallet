#ifndef CERTIFICATEMANAGERINTERFACE_H
#define CERTIFICATEMANAGERINTERFACE_H

#include "certificate.h"
#include "encryptionkey.h"

#include <QByteArray>
#include <QString>

class CertificateManagerInterface
{
public:
    virtual ~CertificateManagerInterface() { }

    virtual bool loadCertificates()                         = 0;
    virtual bool generateCertificates()                     = 0;

    virtual QByteArray      caCertificatePEM() const        = 0;
    virtual QByteArray      caPrivateKeyPEM() const         = 0;
    virtual QByteArray      serverCertificatePEM() const    = 0;
    virtual QByteArray      serverPrivateKeyPEM() const     = 0;

    virtual Certificate *   caCertificate() const           = 0;
    virtual Certificate *   serverCertificate() const       = 0;
    virtual EncryptionKey * caPrivateKey() const            = 0;
    virtual EncryptionKey * serverPrivateKey() const        = 0;
};

#endif // CERTIFICATEMANAGERINTERFACE_H
