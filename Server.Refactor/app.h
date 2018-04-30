#ifndef APP_H
#define APP_H

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

    quint16             restPort() const;                   // 0 if not set

    QString             caCertificateFilename() const;      // (Optional) - If loaded from a file
    QByteArray          caCertificatePEM() const;           // Empty if not valid
    Certificate *       caCertificate() const;              // nullptr if not valid, do NOT delete it

    QString             caPrivateKeyFilename() const;       // (Optional) - If loaded from a file and if exists
    QByteArray          caPrivateKeyPEM() const;            // (Optional) - Empty if not valid
    EncryptionKey *     caPrivateKey() const;               // nullptr if not valid, do NOT delete it

    QString             certificateFilename() const;        // (Optional) - If loaded from a file
    QByteArray          certificatePEM() const;             // Empty if not valid
    Certificate *       certificate() const;                // nullptr if not valid, do NOT delete it

    QString             privateKeyFilename() const;         // (Optional) - If loaded from a file
    QByteArray          privateKeyPEM() const;              // Empty if not valid
    EncryptionKey *     privateKey() const;                 // nullptr if not valid, do NOT delete it

    QString             logsPath() const;                   // Empty if not valid
    QString             recordsPath() const;                // Empty if not valid

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

    // For logging
    void *              mLogManager                 = nullptr;

    // Settings:
    int                 mArgC                       = 0;
    QStringList         mArgV;

    bool                mShouldDoInit                       = false;
    QString             mConfigFile;

    quint16             mRESTPort                   = 0;
    quint16             mRPCPort                    = 0;
    QString             mServerName;
    QString             mServerAddress;             // Only used for init

    QString             mCACertificateFilename;
    QByteArray          mCACertificatePEM;
    Certificate *       mCACertificate              = nullptr;

    QString             mCAPrivateKeyFilename;
    QByteArray          mCAPrivateKeyPEM;
    EncryptionKey *     mCAPrivateKey               = nullptr;

    QString             mCertificateFilename;
    QByteArray          mCertificatePEM;
    Certificate *       mCertificate                = nullptr;

    QString             mPrivateKeyFilename;
    QByteArray          mPrivateKeyPEM;
    EncryptionKey *     mPrivateKey                 = nullptr;

    QString             mLogsPath;
    QString             mRecordsPath;

    QString             mDBUserName;
    QString             mDBPassword;
    QString             mDBName;
    QString             mDBHostName;
    quint16             mDBPort                     = 5432;
    QString             mDBType;

};

#endif // APP_H
