#ifndef APP_H
#define APP_H

#include "config.h"
#include "modulelinker.h"
#include "certificate.h"
#include "encryptionkey.h"

#include <QByteArray>
#include <QString>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QThread>

class CertificateManagerInterface;
class App : public QObject
{
    Q_OBJECT
public:
    static App * globalInstance();
    static int exec(int argc, char *argv[]);

    QByteArray              caCertificatePEM() const;           // Empty if not valid
    Certificate *           caCertificate() const;              // nullptr if not valid, do NOT delete it

    QByteArray              caPrivateKeyPEM() const;            // (Optional) - Empty if not valid
    EncryptionKey *         caPrivateKey() const;               // nullptr if not valid, do NOT delete it

    QByteArray              serverCertificatePEM() const;       // Empty if not valid
    Certificate *           serverCertificate() const;          // nullptr if not valid, do NOT delete it

    QByteArray              serverPrivateKeyPEM() const;        // Empty if not valid
    EncryptionKey *         serverPrivateKey() const;           // nullptr if not valid, do NOT delete it

    const Config *          configuration() const;
    Config *                configuration();

    void                    setCertificates(CertificateManagerInterface * iCertificateManager);

signals:
    void appKernelStarted();
    void modulesLoaded();

public slots:

protected slots:
    void _eventLoopStarted();

protected:
    explicit App(int argc, char *argv[], QObject *iParent = nullptr);

    bool _createModules(bool iForInit = false);
    void _startModules();

    void _parseCommandLine();
    bool _shouldDoInit();
    bool _doInit();
    void _loadConfigurationFile();

    // Modules
    QStringList                 mModulesStartOrder;
    QMap< QString, void * >     mModules;
    QMap< QString, QThread *>   mModuleThreads;
    QMap< QString, bool >       mModuleStarted;

    Config                      mConfiguration;

    QByteArray                  mCACertificatePEM;
    Certificate *               mCACertificate              = nullptr;

    QByteArray                  mCAPrivateKeyPEM;
    EncryptionKey *             mCAPrivateKey               = nullptr;

    QByteArray                  mServerCertificatePEM;
    Certificate *               mServerCertificate          = nullptr;

    QByteArray                  mServerPrivateKeyPEM;
    EncryptionKey *             mServerPrivateKey           = nullptr;
};

#endif // APP_H
