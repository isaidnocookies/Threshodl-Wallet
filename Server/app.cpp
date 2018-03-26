#include "app.h"

#include <QDebug>

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QCoreApplication>

static QByteArray _loadFile(const QString &iFilename)
{
    QFile   lFile{iFilename};

    if( lFile.open(QIODevice::ReadOnly))
        return lFile.readAll();
    return QByteArray();
}

static Certificate * _loadCertificateFilename(const QString &iFilename)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new Certificate{lData};
        }catch(...){
            return nullptr;
        }
    }

    return nullptr;
}

static EncryptionKey * _loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey = true)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new EncryptionKey{lData,iIsPrivateKey};
        }catch(...){
            return nullptr;
        }
    }

    return nullptr;
}

static bool _saveFile(const QByteArray &iData, const QString &iFilename)
{
    QFile lFile{iFilename};

    if( lFile.open(QIODevice::WriteOnly) )
        return lFile.write(iData) >= iData.size();

    return false;
}

App::App(int argc, char *argv[])
    : mArgC(argc)
{
    qInstallMessageHandler(LogsManager::messageHandler);

    for( int lIndex = 0; lIndex < mArgC; lIndex++ ) {
        mArgV << QString::fromLocal8Bit(argv[lIndex]);
    }
}

void App::parseCommandLine()
{
    for( int lIndex = 1; lIndex < mArgC; lIndex++ ) {
        if( mArgV[lIndex] == kCommandLineOption_Init ) {
            mInit = true;
            continue;
        }

        // These options take a parameter
        if( lIndex+1 < mArgC ) {
            if( mArgV[lIndex] == kCommandLineOption_SettingsFile ) {
                lIndex++;
                mConfigFile = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_RESTPort ) {
                lIndex++;
                mRESTPort = static_cast<quint16>(mArgV[lIndex].toUInt() & 0xFFFF);
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_CACertFile ) {
                lIndex++;
                mCACertificateFilename = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_CAKeyFile ) {
                lIndex++;
                mCAPrivateKeyFilename = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_CertificateFile ) {
                lIndex++;
                mCertificateFilename = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_PrivateKeyFile ) {
                lIndex++;
                mPrivateKeyFilename = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_ServerName ) {
                lIndex++;
                mServerName = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_ServerAddress ) {
                lIndex++;
                mServerAddress = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_LogsPath ) {
                lIndex++;
                mLogsPath = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_RecordsPath ) {
                lIndex++;
                mRecordsPath = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBUserName ) {
                lIndex++;
                mDBUserName = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBPassword ) {
                lIndex++;
                mDBPassword = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBName ) {
                lIndex++;
                mDBName = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBHostName ) {
                lIndex++;
                mDBHostName = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBPort ) {
                lIndex++;
                mDBPort = static_cast<quint16>(mArgV[lIndex].toUInt() & 0xFFFF);
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_DBType ) {
                lIndex++;
                mDBType = mArgV[lIndex].toUpper();
                continue;
            }
        }
    }
}

bool App::doInit()
{
    if( mServerAddress.isEmpty() || mServerName.isEmpty() )
        return false;

    QFile           lFile{mConfigFile};
    QVariantMap     lSettings;

    if( mCACertificateFilename.isEmpty() )
        mCACertificateFilename = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("cacert.pem"));

    if( mCAPrivateKeyFilename.isEmpty() )
        mCAPrivateKeyFilename = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("cakey.pem"));

    if( mCertificateFilename.isEmpty() )
        mCertificateFilename = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("cert.pem"));

    if( mPrivateKeyFilename.isEmpty() )
        mPrivateKeyFilename = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("key.pem"));

    if( mLogsPath.isEmpty() )
        mLogsPath = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("logs"));

    if( mRecordsPath.isEmpty() )
        mRecordsPath = QString("%1%2%3").arg(QDir::currentPath()).arg(QDir::separator()).arg(QStringLiteral("records"));

    if( mRESTPort < 1 )
        mRESTPort = 443;

    if( lFile.open(QIODevice::WriteOnly) ) {
        lSettings[kSettingsKey_RESTPort]            = mRESTPort;
        lSettings[kSettingsKey_CACertFile]          = mCACertificateFilename;
        lSettings[kSettingsKey_PrivateKeyFile]      = mPrivateKeyFilename;
        lSettings[kSettingsKey_CertificateFile]     = mCertificateFilename;
        lSettings[kSettingsKey_LogsPath]            = mLogsPath;
        lSettings[kSettingsKey_RecordsPath]         = mRecordsPath;
        lSettings[kSettingsKey_DBUserName]          = mDBUserName;
        lSettings[kSettingsKey_DBPassword]          = mDBPassword;
        lSettings[kSettingsKey_DBHostName]          = mDBHostName;
        lSettings[kSettingsKey_DBPort]              = mDBPort;
        lSettings[kSettingsKey_DBName]              = mDBName;
        lSettings[kSettingsKey_DBType]              = mDBType;
        lFile.write( QJsonDocument::fromVariant(lSettings).toJson() );

        // Init the OpenSSL libraries
        Certificate::initializeLibrary();

        // Load the CA files if they exist
        Certificate *   lCACert = _loadCertificateFilename(mCACertificateFilename);
        EncryptionKey * lCAKey  = _loadEncryptionKeyFilename(mCAPrivateKeyFilename);

        // Load the Certificate and Private key if they exist
        Certificate *   lCert   = _loadCertificateFilename(mCertificateFilename);
        EncryptionKey * lKey    = _loadEncryptionKeyFilename(mPrivateKeyFilename);

        // If the CA files are not valid then create new CA files and save them
        if( lCACert == nullptr || ( lCAKey == nullptr && (lCert == nullptr || lKey == nullptr) ) ) {
            lCACert = new Certificate;
            lCACert->setCADefaults();
            lCACert->selfSign();
            lCAKey = lCACert->encryptionKey();

            if( ! _saveFile( lCACert->toPEM(), mCACertificateFilename ) || ! _saveFile(lCAKey->privateToPEM(), mCAPrivateKeyFilename) )
                return false;
        }

        // Create new Certificate and Private key files if needed and save them (sign using the CA files)
        if( lCert == nullptr || lKey == nullptr ) {
            lCert = new Certificate;
            lCert->setServerDefaults();
            lCert->addServerName(mServerName);
            lCert->addServerName(mServerAddress);
            lCert->setIssuerName( QStringList() << QString("CN=%1").arg(mServerAddress) );
            lCert->setSerialNumber(QDateTime::currentSecsSinceEpoch());
            lCert->sign(lCAKey);

            if( ! _saveFile( lCert->toPEM(), mCertificateFilename ) || ! _saveFile( lCert->encryptionKey()->privateToPEM(), mPrivateKeyFilename ) )
                return false;
        }

        mDBInterface = new DBInterfaceAlpha(mDBUserName,mDBPassword,mDBName,mDBHostName,mDBPort);
        mDBInterface->setSqlType(mDBType);
        return mDBInterface->initDB();
    }

    return false;
}

void App::loadSettings()
{
    QJsonParseError lJsonError;
    QVariantMap     lSettings = QJsonDocument::fromJson(_loadFile(mConfigFile),&lJsonError).toVariant().toMap();

    if( lJsonError.error == QJsonParseError::NoError ) {
        if( lSettings.contains(kSettingsKey_RESTPort) ) {
            mRESTPort = static_cast<quint16>(lSettings[kSettingsKey_RESTPort].toUInt() & 0xFFFF);
        }

        if( lSettings.contains(kSettingsKey_CACertFile) ) {
            mCACertificateFilename = lSettings[kSettingsKey_CACertFile].toString();
        }

        if( lSettings.contains(kSettingsKey_PrivateKeyFile) ) {
            mPrivateKeyFilename = lSettings[kSettingsKey_PrivateKeyFile].toString();
        }

        if( lSettings.contains(kSettingsKey_CertificateFile) ) {
            mCertificateFilename = lSettings[kSettingsKey_CertificateFile].toString();
        }

        if( lSettings.contains(kSettingsKey_LogsPath) ) {
            mLogsPath = lSettings[kSettingsKey_LogsPath].toString();
        }

        if( lSettings.contains(kSettingsKey_RecordsPath) ) {
            mRecordsPath = lSettings[kSettingsKey_RecordsPath].toString();
        }

        if( lSettings.contains(kSettingsKey_DBUserName) ) {
            mDBUserName = lSettings[kSettingsKey_DBUserName].toString();
        }

        if( lSettings.contains(kSettingsKey_DBPassword) ) {
            mDBPassword = lSettings[kSettingsKey_DBPassword].toString();
        }

        if( lSettings.contains(kSettingsKey_DBName) ) {
            mDBName = lSettings[kSettingsKey_DBName].toString();
        }

        if( lSettings.contains(kSettingsKey_DBHostName) ) {
            mDBHostName = lSettings[kSettingsKey_DBHostName].toString();
        }

        if( lSettings.contains(kSettingsKey_DBPort) ) {
            mDBPort = static_cast<quint16>(lSettings[kSettingsKey_DBPort].toUInt() & 0xFFFF);
        }

        if( lSettings.contains(kSettingsKey_DBType) ) {
            mDBType = lSettings[kSettingsKey_DBType].toString().toUpper();
        }
    }
}

void App::loadCryptoFiles()
{
    if( ! mCACertificateFilename.isEmpty() ) {
        if( mCACertificate == nullptr )
            mCACertificate = _loadCertificateFilename(mCACertificateFilename);

        if( mCACertificate )
            mCACertificatePEM = mCACertificate->toPEM();
    }

    if( ! mPrivateKeyFilename.isEmpty() ) {
        if( mPrivateKey == nullptr )
            mPrivateKey = _loadEncryptionKeyFilename(mPrivateKeyFilename);

        if( mPrivateKey )
            mPrivateKeyPEM = mPrivateKey->privateToPEM();
    }

    if( ! mCertificateFilename.isEmpty() ) {
        if( mCertificate == nullptr )
            mCertificate = _loadCertificateFilename(mCertificateFilename);

        if( mCertificate )
            mCertificatePEM = mCertificate->toPEM();
    }
}

void App::start()
{
    mLogsManagerThread = new QThread(this);
    mLogsManager = new LogsManager(mLogsPath);
    mLogsManager->moveToThread(mLogsManagerThread);
    connect( mLogsManagerThread, &QThread::started, mLogsManager, &LogsManager::threadStarted );

    mRecordsManager = new RecordsManager(mRecordsPath);
    mRecordsManagerThread = new QThread(this);
    mRecordsManager->moveToThread(mRecordsManagerThread);
    connect( mRecordsManagerThread, &QThread::started, mRecordsManager, &RecordsManager::threadStarted );

    mDownloaderThread = new QThread(this);
    mDownloader = new Downloader;
    mDownloader->moveToThread(mDownloaderThread);
    connect( mDownloaderThread, &QThread::started, mDownloader, &Downloader::threadStarted );
    connect( mDownloader, &Downloader::downloaded, mRecordsManager, &RecordsManager::handleDownloadedUrlData );

    mDBInterface = new DBInterfaceAlpha{mDBUserName,mDBPassword,mDBName,mDBHostName,mDBPort};

    mLogsManagerThread->start();
    mRecordsManagerThread->start();
    mDownloaderThread->start();

    mRESTHandler = new RESTHandler(this);

    QMetaObject::invokeMethod( this, "eventLoopStarted", Qt::QueuedConnection );
}

bool App::shouldDoInit() const
{
    return mInit;
}

quint16 App::restPort() const
{
    return mRESTPort;
}

QString App::caCertificateFilename() const
{
    return mCACertificateFilename;
}

QByteArray App::caCertificatePEM() const
{
    return mCACertificatePEM;
}

Certificate *App::caCertificate() const
{
    return mCACertificate;
}

QString App::caPrivateKeyFilename() const
{
    return mCAPrivateKeyFilename;
}

QByteArray App::caPrivateKeyPEM() const
{
    return mCAPrivateKeyPEM;
}

EncryptionKey *App::caPrivateKey() const
{
    return mCAPrivateKey;
}

QString App::certificateFilename() const
{
    return mCertificateFilename;
}

QByteArray App::certificatePEM() const
{
    return mCertificatePEM;
}

Certificate *App::certificate() const
{
    return mCertificate;
}

QString App::privateKeyFilename() const
{
    return mPrivateKeyFilename;
}

QByteArray App::privateKeyPEM() const
{
    return mPrivateKeyPEM;
}

EncryptionKey *App::privateKey() const
{
    return mPrivateKey;
}

QString App::logsPath() const
{
    return mLogsPath;
}

LogsManager *App::logsManager() const
{
    return mLogsManager;
}

QString App::recordsPath() const
{
    return mRecordsPath;
}

RecordsManager *App::recordsManager() const
{
    return mRecordsManager;
}

DBInterface *App::databaseInterface() const
{
    return mDBInterface;
}

void App::startHTTPS()
{
    mHttpsSettings = new QSettings(this);
    mHttpsSettings->beginGroup(QStringLiteral("https"));
    mHttpsSettings->setValue("port",mRESTPort);
    mHttpsSettings->setValue("minThreads",QThread::idealThreadCount() * 4);
    mHttpsSettings->setValue("maxThreads",QThread::idealThreadCount() * 40);
    mHttpsSettings->setValue("cleanupInterval",1000);
    mHttpsSettings->setValue("readTimeout",20000);
    mHttpsSettings->setValue("maxRequestSize",81920);
    mHttpsSettings->setValue("maxMultipartSize",4194304);
    mHttpsSettings->setValue("sslKeyFile",mPrivateKeyFilename);
    mHttpsSettings->setValue("sslCertFile",mCertificateFilename);
    mHttpsSettings->setValue("sslCACertFile",mCACertificateFilename);

    mRESTHandler = new RESTHandler( this );
    mHttpsListener = new HttpListener( mHttpsSettings, mRESTHandler, this );
}

void App::eventLoopStarted()
{
    startHTTPS();
}
