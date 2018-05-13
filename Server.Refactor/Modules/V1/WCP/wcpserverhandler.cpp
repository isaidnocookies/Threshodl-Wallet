#include "wcpserverhandler.h"
#include "wcpclient.h"
#include "app.h"

#include <QDebug>
#include <QSslKey>

static WCPServerHandlerML _gRegisterModuleLinker;

WCPServerHandler::WCPServerHandler(QObject *iParent)
    : WCPServerHandlerInterface(iParent)
{ }

WCPServerHandler::~WCPServerHandler()
{ }

void WCPServerHandler::serverStarted()
{
    qInfo() << "Wallet Core Protocol (WCP) Server started on port" << mPort;
}

void WCPServerHandler::serverStopped()
{
    qWarning() << "Wallet Core Protocol (WCP) Server STOPPED on port" << mPort;
}

void WCPServerHandler::serverFailedToStart()
{
    qCritical() << "Wallet Core Protocol (WCP) Server FAILED to start on port" << mPort;
}

void WCPServerHandler::serverFailedToStop()
{
    qCritical() << "Wallet Core Protocol (WCP) Server FAILED to STOP on port" << mPort;
}

void WCPServerHandler::newConnectionArrived()
{
    qDebug() << "Wallet Core Protocol (WCP) Server received a new client connection on port" << mPort;
    auto lConnection = getWCPServer()->nextConnection();
    if( lConnection ) {
        _addActiveClient(new WCPClient{lConnection, this, this});
        qDebug() << "Accepted new client connection.";
    }else{
        qInfo() << "Received stale new client connection notification, ignoring it.";
    }
}

WCPServerHandlerML::WCPServerHandlerML()
{
    QStringList lDependencies = QStringList() << QStringLiteral("Database-1") << QStringLiteral("Grinder-1") << QStringLiteral("FeeEstimator-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("WCPServerHandler-1"),lDependencies,WCPServerHandlerML::creator,WCPServerHandlerML::doInit,WCPServerHandlerML::start,WCPServerHandlerML::startInOwnThread);
}

void *WCPServerHandlerML::creator(void *pointerToAppObject)
{
    App *                   lApp                = reinterpret_cast<App *>(pointerToAppObject);
    auto                    lConfig             = lApp->configuration();
    QHostAddress            lListenAddress      = QHostAddress::Any;
    DatabaseInterface *     lDB                 = reinterpret_cast<DatabaseInterface *>(lApp->getModuleObject(QStringLiteral("Database-1")));
    GrinderInterface *      lGrinder            = reinterpret_cast<GrinderInterface *>(lApp->getModuleObject(QStringLiteral("Grinder-1")));
    FeeEstimatorInterface * lFeeEstimator       = reinterpret_cast<FeeEstimatorInterface *>(lApp->getModuleObject(QStringLiteral("FeeEstimator-1")));

    if(
        ! lDB                                               ||
        ! lGrinder                                          ||
        ! lFeeEstimator                                     ||
        ! lConfig->contains(QStringLiteral("WCPPort"))      ||
        ! lConfig->contains(QStringLiteral("ServerName"))   ||
        lApp->caCertificatePEM().isEmpty()                  ||
        lApp->serverCertificatePEM().isEmpty()              ||
        lApp->serverPrivateKeyPEM().isEmpty()
    ) {
        return nullptr;
    }

    if( lConfig->contains(QStringLiteral("WCPListenAddress")) )
    {
        lListenAddress = QHostAddress(lConfig->toString(QStringLiteral("WCPListenAddress")));
    }

    QSslConfiguration   lSslConfig = QSslConfiguration::defaultConfiguration();
    lSslConfig.setCaCertificates( lSslConfig.caCertificates() << QSslCertificate{lApp->caCertificatePEM(), QSsl::Pem} );
    lSslConfig.setPrivateKey( QSslKey{lApp->serverPrivateKeyPEM(), QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, QByteArray()} );
    lSslConfig.setLocalCertificate( QSslCertificate{lApp->serverCertificatePEM(), QSsl::Pem} );

    WCPServerHandler *  lServer = new WCPServerHandler;
    lServer->mApp               = lApp;
    lServer->mPort              = lConfig->toUInt16(QStringLiteral("WCPPort"));
    lServer->mServerNames       = lConfig->toStringList(QStringLiteral("ServerName"));
    lServer->mListenAddress     = lListenAddress;
    lServer->mSslConfiguration  = lSslConfig;
    lServer->mDB                = lDB;
    lServer->mGrinder           = lGrinder;
    lServer->mFeeEstimator      = lFeeEstimator;
    return lServer;
}

bool WCPServerHandlerML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    Q_UNUSED(pointerToThis)
    return true;
}

bool WCPServerHandlerML::startInOwnThread()
{ return true; }

bool WCPServerHandlerML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)

    WCPServerHandler *  lServer = reinterpret_cast<WCPServerHandler *>(pointerToThis);
    lServer->setWCPServer(new WCPServer{lServer});
    lServer->getWCPServer()->startListening(lServer->mListenAddress,lServer->mPort,lServer->mServerNames.first(),lServer->mSslConfiguration);
    return true;
}
