#ifndef CERTIFICATEMANAGERV1_H
#define CERTIFICATEMANAGERV1_H

#include "../../Interface/CertificateManagerInterface/certificatemanagerinterface.h"

#include "certificate.h"
#include "encryptionkey.h"

#include <QByteArray>
#include <QString>

class App;
class CertificateManagerML;
class CertificateManager : public CertificateManagerInterface
{
    friend class CertificateManagerML;

protected:
    App *                   mApp                    = nullptr;

    static bool             _saveFile(const QByteArray &iData, const QString &iFilename);

    static QByteArray       _loadFile(const QString &iFilename);
    static Certificate *    _loadCertificateFilename(const QString &iFilename);
    static EncryptionKey *  _loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey = true);

    static bool             _loadCertificatePEMOrFile(QByteArray &ioPEM, const QString iFilename, Certificate ** oCertificate);
    static bool             _loadEncryptionKeyPEMOrFile(QByteArray &ioPEM, const QString iFilename, EncryptionKey ** oKey, bool iIsPrivateKey = true);

public:
    virtual ~CertificateManager();

    CertificateManager()
    { }

    void loadConfigurationValues();

    bool loadCertificates() override;
    bool generateCertificates() override;

    QString             CACertificateFilename;
    Certificate *       CACertificate               = nullptr;
    QByteArray          CACertificatePEM;

    QString             ServerCertificateFilename;
    Certificate *       ServerCertificate           = nullptr;
    QByteArray          ServerCertificatePEM;

    QString             ServerPrivateKeyFilename;
    EncryptionKey *     ServerPrivateKey            = nullptr;
    QByteArray          ServerPrivateKeyPEM;

    QString             CAPrivateKeyFilename;                   // Optional
    EncryptionKey *     CAPrivateKey                = nullptr;  // Optional
    QByteArray          CAPrivateKeyPEM;                        // Optional
};

class CertificateManagerML
{
public:
    CertificateManagerML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // CERTIFICATEMANAGERV1_H
