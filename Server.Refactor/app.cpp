#include "app.h"
#include "Modules/Interface/CertificateManagerInterface/certificatemanagerinterface.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QVariant>
#include <QJsonDocument>
#include <QSslKey>
#include <QCoreApplication>
#include <QAbstractEventDispatcher>

#ifdef  UNIT_TEST_ON
void UnitTestInjectCommandConfiguration(Config * iConfiguration);
#else
#   define      UnitTestInjectCommandConfiguration(x)
#endif  // UNIT_TESTS_ON

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

QByteArray App::serverCertificatePEM() const
{ return mServerCertificatePEM; }

Certificate *App::serverCertificate() const
{ return mServerCertificate; }

QByteArray App::serverPrivateKeyPEM() const
{ return mServerPrivateKeyPEM; }

EncryptionKey *App::serverPrivateKey() const
{ return mServerPrivateKey; }

const Config *App::configuration() const
{ return &mConfiguration; }

Config *App::configuration()
{ return &mConfiguration; }

void App::setCertificates(CertificateManagerInterface *iCertificateManager)
{
    mCACertificate      = mServerCertificate    = nullptr;
    mCAPrivateKey       = mServerPrivateKey     = nullptr;
    mCACertificatePEM   = mCAPrivateKeyPEM      = mServerCertificatePEM     = mServerPrivateKeyPEM  = QByteArray();

    if( iCertificateManager ) return;

    mCACertificate              = iCertificateManager->caCertificate();
    mCACertificatePEM           = iCertificateManager->caCertificatePEM();
    mCAPrivateKey               = iCertificateManager->caPrivateKey();
    mCAPrivateKeyPEM            = iCertificateManager->caPrivateKeyPEM();
    mServerCertificate          = iCertificateManager->serverCertificate();
    mServerCertificatePEM       = iCertificateManager->serverCertificatePEM();
    mServerPrivateKey           = iCertificateManager->serverPrivateKey();
    mServerPrivateKeyPEM        = iCertificateManager->serverPrivateKeyPEM();
}

bool App::isModuleLinked(const QString iModuleName) const
{ return mModules.contains(iModuleName); }

void *App::getModuleObject(const QString iModuleName) const
{ return mModules[iModuleName]; }

QThread *App::getModuleThread(const QString iModuleName) const
{ return mModuleThreads[iModuleName]; }

bool App::isModuleStarted(const QString iModuleName) const
{ return mModuleStarted[iModuleName]; }

QStringList App::getModuleLoadAndStartOrder() const
{ return mModulesStartOrder; }

bool App::_createModules(bool iForInit)
{
    ModuleLinker::preventNewRegistrationOfModules();

    QList<ModuleLinker::ModuleInfo *> lModules = ModuleLinker::sortRegisteredModulesByDependencies(ModuleLinker::registeredModules());

    qDebug() << "Resolved Internal Module load and start order:";
    for( auto lE : lModules )
    {
        qDebug().noquote().nospace() << "[" << ((lE->ThreadStart && lE->ThreadStart()) ? "s" : "M") << "] " << lE->Name;
    }

    if( ! lModules.isEmpty() ) {
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

    int lArgC = argc;

    for( int lIndex = 0; lIndex < lArgC; lIndex++ ) {
        lArgV << QString::fromLocal8Bit(argv[lIndex]);
    }

    mConfiguration.setValue(QStringLiteral("CommandLineArgumentCount"), lArgC);
    mConfiguration.setValue(QStringLiteral("CommandLineArguments"), lArgV);

    UnitTestInjectCommandConfiguration(&mConfiguration);
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

        // Is it a Json value?
        bool lStoreIt = true;
        if( lValue.startsWith(QChar{'{'}) || lValue.startsWith(QChar{'['}) ) {
            // Its probably a Json value
            QJsonParseError lError;
            QVariant        lVar = QJsonDocument::fromJson(lValue.toUtf8(),&lError).toVariant();
            if( lError.error == QJsonParseError::NoError ) {
                mConfiguration.setValue(lKey,lVar);
                lStoreIt = false;
            }
        }

        if( lStoreIt )
            mConfiguration.setValue(lKey,lValue);
    }
}

bool App::_shouldDoInit()
{ return (mConfiguration.toInt32(QStringLiteral("DoInit")) != 0); }

bool App::_doInit()
{
    mConfiguration.removeValue(QStringLiteral("DoInit"));

    _createModules(true);

    for( QString lModuleName : mModulesStartOrder )
    {
        ModuleLinker::ModuleInfo * lMI = ModuleLinker::moduleInfoForName(lModuleName);
        qDebug() << "Initializing module" << lMI->Name;
        if( lMI->DoInit )
            if( ! lMI->DoInit(mModules[lModuleName], this) ) {
                return false;
            }
    }

    mConfiguration.save();

    return true;
}

void App::_loadConfigurationFile()
{
    if( mConfiguration.contains(QStringLiteral("ConfigurationFile")) )
        mConfiguration.setFilename(mConfiguration.value(QStringLiteral("ConfigurationFile")).toString());
    else if( mConfiguration.contains(QStringLiteral("ConfigFile")) )
        mConfiguration.setFilename(mConfiguration.value(QStringLiteral("ConfigFile")).toString());

    if( mConfiguration.filename().isEmpty() )
        mConfiguration.setFilename(kConfigFile);

    if( ! mConfiguration.load() )
        qWarning() << "Failed to load configuration file!";
}
