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

class App : public QObject
{
    Q_OBJECT
public:
    static App * globalInstance();
    static int exec(int argc, char *argv[]);

    QByteArray          caCertificatePEM() const;           // Empty if not valid
    Certificate *       caCertificate() const;              // nullptr if not valid, do NOT delete it

    QByteArray          caPrivateKeyPEM() const;            // (Optional) - Empty if not valid
    EncryptionKey *     caPrivateKey() const;               // nullptr if not valid, do NOT delete it

    QByteArray          certificatePEM() const;             // Empty if not valid
    Certificate *       certificate() const;                // nullptr if not valid, do NOT delete it

    QByteArray          privateKeyPEM() const;              // Empty if not valid
    EncryptionKey *     privateKey() const;                 // nullptr if not valid, do NOT delete it

    const Config *      configuration() const;
    Config *            configuration();

    void *              logManager() const;                 // For internal logging, do not use directly
    void                setLogManager(void * iLogManager);  // For internal logging, do not use directly

signals:
    void appKernelStarted();
    void modulesLoaded();

public slots:

protected slots:
    void _eventLoopStarted();

protected:
    explicit App(int argc, char *argv[], QObject *iParent = nullptr);

    static QByteArray _loadFile(const QString &iFilename);
    static Certificate * _loadCertificateFilename(const QString &iFilename);
    static EncryptionKey * _loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey = true);
    static bool _saveFile(const QByteArray &iData, const QString &iFilename);

    bool _createModules(bool iForInit = false);
    void _startModules();

    void _parseCommandLine();
    bool _shouldDoInit();
    bool _doInit();
    void _loadConfigurationFile();
    void _loadCryptoFiles();

    // Modules
    QStringList                 mModulesStartOrder;
    QMap< QString, void * >     mModules;
    QMap< QString, QThread *>   mModuleThreads;
    QMap< QString, bool >       mModuleStarted;

    Config                      mConfiguration;

    // For logging
    void *                      mLogManager                 = nullptr;

    QByteArray                  mCACertificatePEM;
    Certificate *               mCACertificate              = nullptr;

    QByteArray                  mCAPrivateKeyPEM;
    EncryptionKey *             mCAPrivateKey               = nullptr;

    QByteArray                  mCertificatePEM;
    Certificate *               mCertificate                = nullptr;

    QByteArray                  mPrivateKeyPEM;
    EncryptionKey *             mPrivateKey                 = nullptr;
};

#endif // APP_H
