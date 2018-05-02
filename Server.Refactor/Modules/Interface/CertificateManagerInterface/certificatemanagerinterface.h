#ifndef CERTIFICATEMANAGERINTERFACE_H
#define CERTIFICATEMANAGERINTERFACE_H

#include "certificate.h"

#include <QString>

class CertificateManagerInterface
{
public:
    virtual ~CertificateManagerInterface() { }

    virtual void generateCertificates() = 0;
};

#endif // CERTIFICATEMANAGERINTERFACE_H
