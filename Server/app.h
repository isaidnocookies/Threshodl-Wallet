#ifndef APP_H
#define APP_H

#include "main.h"
#include "certificate.h"
#include "downloader.h"
#include "logsmanager.h"
#include "recordsmanager.h"
#include "dbinterfacealpha.h"
#include "httplistener.h"
#include "resthandler.h"

#include <QMutex>
#include <QByteArray>
#include <QString>
#include <QObject>
#include <QThread>
#include <qsettings.h>

#define     kSettingsFile                           "config.json"

#define     kCommandLineOption_Init                 "-init"
#define     kCommandLineOption_SettingsFile         "-conf"
#define     kCommandLineOption_RESTPort             "-port"
#define     kCommandLineOption_CACertFile           "-caCert"
#define     kCommandLineOption_CAKeyFile            "-caKey"
#define     kCommandLineOption_CertificateFile      "-cert"
#define     kCommandLineOption_PrivateKeyFile       "-key"
#define     kCommandLineOption_ServerName           "-serverName"
#define     kCommandLineOption_ServerAddress        "-serverAddress"
#define     kCommandLineOption_LogsPath             "-logs"
#define     kCommandLineOption_RecordsPath          "-records"
#define     kCommandLineOption_DBUserName           "-dbusername"
#define     kCommandLineOption_DBPassword           "-dbpassword"
#define     kCommandLineOption_DBName               "-dbname"
#define     kCommandLineOption_DBHostName           "-dbhostname"
#define     kCommandLineOption_DBPort               "-dbport"
#define     kCommandLineOption_DBType               "-dbtype"

#define     kSettingsKey_RESTPort                   "RESTPort"
#define     kSettingsKey_CACertFile                 "CACertFile"
#define     kSettingsKey_PrivateKeyFile             "PrivateKeyFile"
#define     kSettingsKey_CertificateFile            "CertificateFile"
#define     kSettingsKey_LogsPath                   "LogsPath"
#define     kSettingsKey_RecordsPath                "RecordsPath"
#define     kSettingsKey_DBUserName                 "DBUserName"
#define     kSettingsKey_DBPassword                 "DBPassword"
#define     kSettingsKey_DBName                     "DBName"
#define     kSettingsKey_DBHostName                 "DBHostName"
#define     kSettingsKey_DBPort                     "DBPort"
#define     kSettingsKey_DBType                     "DBType"

class App : public QObject
{
    Q_OBJECT

public:
    explicit            App(int argc, char *argv[]);
    void                parseCommandLine();                 // Step 1 and Step 3
    bool                doInit();                           // Step 2 (If init is true) then exit, false only happens when something really bad occurs or something critical is missing
    void                loadSettings();                     // Step 2
    void                loadCryptoFiles();                  // Step 4
    void                start();                            // Step 5

    bool                shouldDoInit() const;               // returns mInit value

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
    LogsManager *       logsManager() const;

    QString             recordsPath() const;                // Empty if not valid
    RecordsManager *    recordsManager() const;

    DBInterface *       databaseInterface() const;

public slots:
    void                eventLoopStarted();

protected:
    void                startHTTPS();

    int                 mArgC                       = 0;
    QStringList         mArgV;

    bool                mInit                       = false;
    QString             mConfigFile                 = kSettingsFile;

    quint16             mRESTPort                   = 0;
    QString             mServerName;                // Only used for init
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
    QThread *           mLogsManagerThread          = nullptr;
    LogsManager *       mLogsManager                = nullptr;

    QString             mRecordsPath;
    QThread *           mRecordsManagerThread       = nullptr;
    RecordsManager *    mRecordsManager             = nullptr;

    QThread *           mDownloaderThread           = nullptr;
    Downloader *        mDownloader                 = nullptr;

    QSettings *         mHttpsSettings              = nullptr;
    HttpListener *      mHttpsListener              = nullptr;
    RESTHandler *       mRESTHandler                = nullptr;

    QString             mDBUserName;
    QString             mDBPassword;
    QString             mDBName;
    QString             mDBHostName;
    quint16             mDBPort                     = 5432;
    QString             mDBType;

    DBInterface *       mDBInterface                = nullptr;
};

#endif // APP_H
