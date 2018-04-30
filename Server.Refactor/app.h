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
