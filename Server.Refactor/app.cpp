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

QByteArray App::caCertificatePEM() const
{ return mCACertificatePEM; }

Certificate *App::caCertificate() const
{ return mCACertificate; }

QByteArray App::caPrivateKeyPEM() const
{ return mCAPrivateKeyPEM; }

EncryptionKey *App::caPrivateKey() const
{ return mCAPrivateKey; }

QByteArray App::certificatePEM() const
{ return mCertificatePEM; }

Certificate *App::certificate() const
{ return mCertificate; }

QByteArray App::privateKeyPEM() const
{ return mPrivateKeyPEM; }

EncryptionKey *App::privateKey() const
{ return mPrivateKey; }

const Config *App::configuration() const
{ return &mConfiguration; }

Config *App::configuration()
{ return &mConfiguration; }

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
{
    mConfiguration.setFilename(kConfigFile);
    QStringList lArgV;

    for( int lIndex = 0; lIndex < argc; lIndex++ ) {
        lArgV << QString::fromLocal8Bit(argv[lIndex]);
    }

    mConfiguration.setValue(QStringLiteral("CommandLineArgumentCount"), argc);
    mConfiguration.setValue(QStringLiteral("CommandLineArguments"), lArgV);
}

void App::_parseCommandLine()
{
    QStringList lCommandLineArguments = mConfiguration.toStringList(QStringLiteral("CommandLineArguments"));

    for( int lIndex = 1; lIndex < mConfiguration.toInt32(QStringLiteral("CommandLineArgumentCount")); lIndex++ ) {
        // Format:
        // Key=Value....

        QString     lArgument           = lCommandLineArguments.at(lIndex);
        int         lLocationOfEquals   = lArgument.indexOf(QChar('='));

        if( lLocationOfEquals < 0 ) continue;

        QString     lKey                = lArgument.left(lLocationOfEquals);
        QString     lValue              = lArgument.mid(lLocationOfEquals+1);

        if( lKey.toLower() == QStringLiteral("configfile") || lKey == QStringLiteral("ConfigurationFilename") ) {
            mConfiguration.setFilename(lValue);
            continue;
        }

        mConfiguration.setValue(lKey,lValue);
    }
}

bool App::_shouldDoInit()
{ return (mConfiguration.toInt32(QStringLiteral("DoInit")) != 0); }

bool App::_doInit()
{
    mConfiguration.removeValue(QStringLiteral("DoInit"));

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
    if( mConfiguration.filename().isEmpty() )
        mConfiguration.setFilename(kConfigFile);

    if( ! mConfiguration.load() )
        qWarning() << "Failed to load configuration file!";
}

void App::_loadCryptoFiles()
{
    QString lCACertificateFilename = mConfiguration.toString(QStringLiteral("CACertificateFilename"));
    QString lPrivateKeyFilename = mConfiguration.toString(QStringLiteral("PrivateKeyFilename"));
    QString lCertificateFilename = mConfiguration.toString(QStringLiteral("CertificateFilename"));

    if( ! lCACertificateFilename.isEmpty() ) {
        if( mCACertificate == nullptr )
            mCACertificate = _loadCertificateFilename(lCACertificateFilename);

        if( mCACertificate )
            mCACertificatePEM = mCACertificate->toPEM();
    }

    if( ! lPrivateKeyFilename.isEmpty() ) {
        if( mPrivateKey == nullptr )
            mPrivateKey = _loadEncryptionKeyFilename(lPrivateKeyFilename);

        if( mPrivateKey )
            mPrivateKeyPEM = mPrivateKey->privateToPEM();
    }

    if( ! lCertificateFilename.isEmpty() ) {
        if( mCertificate == nullptr )
            mCertificate = _loadCertificateFilename(lCertificateFilename);

        if( mCertificate )
            mCertificatePEM = mCertificate->toPEM();
    }
}
