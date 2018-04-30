#include "app.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QVariant>
#include <QJsonDocument>
#include <QSslKey>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

#define     kConfigFile                             "config.json"

#define     kCommandLineOption_Init                 "-init"
#define     kCommandLineOption_ConfigFile           "-config"
#define     kCommandLineOption_RESTPort             "-restPort"
#define     kCommandLineOption_RPCPort              "-rpcPort"
#define     kCommandLineOption_CACertFile           "-caCert"
#define     kCommandLineOption_CAKeyFile            "-caKey"
#define     kCommandLineOption_CertificateFile      "-cert"
#define     kCommandLineOption_PrivateKeyFile       "-key"
#define     kCommandLineOption_ServerName           "-serverName"
#define     kCommandLineOption_ServerAddress        "-serverAddress"
#define     kCommandLineOption_LogsPath             "-logs"
#define     kCommandLineOption_RecordsPath          "-records"
#define     kCommandLineOption_DBUserName           "-dbUserName"
#define     kCommandLineOption_DBPassword           "-dbPassword"
#define     kCommandLineOption_DBName               "-dbName"
#define     kCommandLineOption_DBHostName           "-dbHostName"
#define     kCommandLineOption_DBPort               "-dbPort"
#define     kCommandLineOption_DBType               "-dbYype"

#define     kConfigKey_RESTPort                     "RESTPort"
#define     kConfigKey_RPCPort                      "RPCPort"
#define     kConfigKey_ServerName                   "ServerName"
#define     kConfigKey_CACertFile                   "CACertFile"
#define     kConfigKey_PrivateKeyFile               "PrivateKeyFile"
#define     kConfigKey_CertificateFile              "CertificateFile"
#define     kConfigKey_LogsPath                     "LogsPath"
#define     kConfigKey_RecordsPath                  "RecordsPath"
#define     kConfigKey_DBUserName                   "DBUserName"
#define     kConfigKey_DBPassword                   "DBPassword"
#define     kConfigKey_DBName                       "DBName"
#define     kConfigKey_DBHostName                   "DBHostName"
#define     kConfigKey_DBPort                       "DBPort"
#define     kConfigKey_DBType                       "DBType"

static App * _globalInstance = nullptr;

// https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style
template <typename F>
static void postToThread(F && fun, QThread * thread = qApp->thread()) {
   QObject * obj = QAbstractEventDispatcher::instance(thread);
   Q_ASSERT(obj);
   QObject src;
   QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(fun),
                    Qt::QueuedConnection);
}

template <typename F>
static void postToThreadAndWait(F && fun, QThread * thread = qApp->thread()) {
   QObject * obj = QAbstractEventDispatcher::instance(thread);
   Q_ASSERT(obj);
   QObject src;
   QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(fun),
                    Qt::BlockingQueuedConnection);
}

App *App::globalInstance()
{ return _globalInstance; }

int App::exec(int argc, char *argv[])
{
    QCoreApplication lApp(argc, argv);
    lApp.setApplicationName(QStringLiteral("Threshodl"));
    lApp.setOrganizationName(QStringLiteral("Threshodl"));
    lApp.setOrganizationDomain(QStringLiteral("com.threshodl"));

    _globalInstance = new App{argc,argv};
    _globalInstance->_parseCommandLine();
    if( _globalInstance->_shouldDoInit() ) {
        if( _globalInstance->_doInit() )
            return 0;
        return 1;
    }
    _globalInstance->_loadConfigurationFile();
    _globalInstance->_parseCommandLine();
    _globalInstance->_loadCryptoFiles();

    QMetaObject::invokeMethod( _globalInstance, "_eventLoopStarted", Qt::QueuedConnection );

    return lApp.exec();
}

QString App::caCertificateFilename() const
{ return mCACertificateFilename; }

QByteArray App::caCertificatePEM() const
{ return mCACertificatePEM; }

Certificate *App::caCertificate() const
{ return mCACertificate; }

QString App::caPrivateKeyFilename() const
{ return mCAPrivateKeyFilename; }

QByteArray App::caPrivateKeyPEM() const
{ return mCAPrivateKeyPEM; }

EncryptionKey *App::caPrivateKey() const
{ return mCAPrivateKey; }

QString App::certificateFilename() const
{ return mCertificateFilename; }

QByteArray App::certificatePEM() const
{ return mCertificatePEM; }

Certificate *App::certificate() const
{ return mCertificate; }

QString App::privateKeyFilename() const
{ return mPrivateKeyFilename; }

QByteArray App::privateKeyPEM() const
{ return mPrivateKeyPEM; }

EncryptionKey *App::privateKey() const
{ return mPrivateKey; }

QString App::logsPath() const
{ return mLogsPath; }

QString App::recordsPath() const
{ return mRecordsPath; }

void *App::logManager() const
{ return mLogManager; }

void App::setLogManager(void *iLogManager)
{ mLogManager = iLogManager; }

QByteArray App::_loadFile(const QString &iFilename)
{
    QFile   lFile{iFilename};

    if( lFile.open(QIODevice::ReadOnly))
        return lFile.readAll();
    return QByteArray();
}

Certificate * App::_loadCertificateFilename(const QString &iFilename)
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

EncryptionKey * App::_loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey)
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

bool App::_saveFile(const QByteArray &iData, const QString &iFilename)
{
    QFile lFile{iFilename};

    if( lFile.open(QIODevice::WriteOnly) )
        return lFile.write(iData) >= iData.size();

    return false;
}

bool App::_createModules(bool iForInit)
{
    ModuleLinker::preventNewRegistrationOfModules();

    QList<ModuleLinker::ModuleInfo *> lModules = ModuleLinker::sortRegisteredModulesByDependencies(ModuleLinker::registeredModules());

    if( ! lModules.isEmpty() ) {

        // First create the modules

        // Honor threads if iForInit is false
        for( auto lE : lModules ) {
            bool lShouldStartInThread = false;

            if( ! iForInit && lE->ThreadStart )
                lShouldStartInThread = lE->ThreadStart();

            if( iForInit )
                qDebug() << "Creating module" << lE->Name;

            mModulesStartOrder << lE->Name;
            mModuleStarted[lE->Name] = false;

            if( lShouldStartInThread ) {
                ModuleLinker::ModuleInfo * lModuleInfo = lE;
                QThread * lNewThread = new QThread{this};
                connect( lNewThread, &QThread::started, [=](){
                    mModules[lModuleInfo->Name] = lModuleInfo->Creator(this);
                    mModuleThreads[lModuleInfo->Name] = lNewThread;
                });
                lNewThread->start();
            } else {
                // Run in main thread
                mModules[lE->Name]          = lE->Creator(this);
                mModuleThreads[lE->Name]    = nullptr;
            }
        }

        return true;
    }

    return false;
}

void App::_startModules()
{
    for( auto lModuleName : mModulesStartOrder ) {
        ModuleLinker::ModuleInfo *  lMI     = ModuleLinker::moduleInfoForName(lModuleName);
        QThread *                   lThread = mModuleThreads[lModuleName];

        if( lThread ) {
            // Execute start in that thread
            if( ! mModuleStarted[lModuleName] ) {
                if( lMI->Start ) {
                    bool lResult = false;
                    bool * lResultPtr = &lResult;
                    postToThreadAndWait([=]{*lResultPtr = lMI->Start(mModules[lModuleName], this);});
                    mModuleStarted[lModuleName] = lResult;
                } else {
                    mModuleStarted[lModuleName] = true;
                }
            }
        } else {
            if( ! mModuleStarted[lModuleName] ) {
                if( lMI->Start )
                    mModuleStarted[lModuleName] = lMI->Start(mModules[lModuleName], this);
                else
                    mModuleStarted[lModuleName] = true;
            }
        }
    }
}

void App::_eventLoopStarted()
{
    emit appKernelStarted();

    // Load Modules
    if( _createModules(false) ) {
        emit modulesLoaded();
    }else{
        qFatal("Unable to load internal modules!");
    }
}

App::App(int argc, char *argv[], QObject *iParent)
    : QObject(iParent)
    , mArgC(argc)
    , mConfigFile(kConfigFile)
{
    for( int lIndex = 0; lIndex < mArgC; lIndex++ ) {
        mArgV << QString::fromLocal8Bit(argv[lIndex]);
    }
}

void App::_parseCommandLine()
{
    for( int lIndex = 1; lIndex < mArgC; lIndex++ ) {
        if( mArgV[lIndex] == kCommandLineOption_Init ) {
            mShouldDoInit = true;
            continue;
        }

        // These options take a parameter
        if( lIndex+1 < mArgC ) {
            if( mArgV[lIndex] == kCommandLineOption_ConfigFile ) {
                lIndex++;
                mConfigFile = mArgV[lIndex];
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_RESTPort ) {
                lIndex++;
                mRESTPort = static_cast<quint16>(mArgV[lIndex].toUInt() & 0xFFFF);
                continue;
            }

            if( mArgV[lIndex] == kCommandLineOption_RPCPort ) {
                lIndex++;
                mRPCPort = static_cast<quint16>(mArgV[lIndex].toUInt() & 0xFFFF);
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

bool App::_shouldDoInit()
{ return mShouldDoInit; }

bool App::_doInit()
{
    _createModules(true);

    for( QString lModuleName : mModules.keys() )
    {
        ModuleLinker::ModuleInfo * lMI = ModuleLinker::moduleInfoForName(lModuleName);
        qDebug() << "Initializing module" << lMI->Name;
        if( lMI->DoInit )
            if( ! lMI->DoInit(mModules[lModuleName], this) ) {
                return false;
            }
    }

    return true;
}

void App::_loadConfigurationFile()
{
    QJsonParseError lJsonError;
    QVariantMap     lSettings = QJsonDocument::fromJson(_loadFile(mConfigFile),&lJsonError).toVariant().toMap();

    if( lJsonError.error == QJsonParseError::NoError ) {
        if( lSettings.contains(kConfigKey_RESTPort) ) {
            mRESTPort = static_cast<quint16>(lSettings[kConfigKey_RESTPort].toUInt() & 0xFFFF);
        }

        if( lSettings.contains(kConfigKey_RPCPort) ) {
            mRPCPort = static_cast<quint16>(lSettings[kConfigKey_RPCPort].toUInt() & 0xFFFF);
        }

        if( lSettings.contains(kConfigKey_ServerName) ) {
            mServerName = lSettings[kConfigKey_ServerName].toString();
        }

        if( lSettings.contains(kConfigKey_CACertFile) ) {
            mCACertificateFilename = lSettings[kConfigKey_CACertFile].toString();
        }

        if( lSettings.contains(kConfigKey_PrivateKeyFile) ) {
            mPrivateKeyFilename = lSettings[kConfigKey_PrivateKeyFile].toString();
        }

        if( lSettings.contains(kConfigKey_CertificateFile) ) {
            mCertificateFilename = lSettings[kConfigKey_CertificateFile].toString();
        }

        if( lSettings.contains(kConfigKey_LogsPath) ) {
            mLogsPath = lSettings[kConfigKey_LogsPath].toString();
        }

        if( lSettings.contains(kConfigKey_RecordsPath) ) {
            mRecordsPath = lSettings[kConfigKey_RecordsPath].toString();
        }

        if( lSettings.contains(kConfigKey_DBUserName) ) {
            mDBUserName = lSettings[kConfigKey_DBUserName].toString();
        }

        if( lSettings.contains(kConfigKey_DBPassword) ) {
            mDBPassword = lSettings[kConfigKey_DBPassword].toString();
        }

        if( lSettings.contains(kConfigKey_DBName) ) {
            mDBName = lSettings[kConfigKey_DBName].toString();
        }

        if( lSettings.contains(kConfigKey_DBHostName) ) {
            mDBHostName = lSettings[kConfigKey_DBHostName].toString();
        }

        if( lSettings.contains(kConfigKey_DBPort) ) {
            mDBPort = static_cast<quint16>(lSettings[kConfigKey_DBPort].toUInt() & 0xFFFF);
        }

        if( lSettings.contains(kConfigKey_DBType) ) {
            mDBType = lSettings[kConfigKey_DBType].toString().toUpper();
        }
    }
}

void App::_loadCryptoFiles()
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
