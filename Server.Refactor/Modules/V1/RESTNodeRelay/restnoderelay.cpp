#include "restnoderelay.h"
#include "app.h"
#include "modulelinker.h"

static RESTNodeRelayML _gRegisterModuleLinker;

RESTNodeRelayML::RESTNodeRelayML()
{ ModuleLinker::registerModule(QStringLiteral("RESTNodeRelay-1"),RESTNodeRelayML::creator,RESTNodeRelayML::doInit,RESTNodeRelayML::start,RESTNodeRelayML::startInOwnThread); }

void *RESTNodeRelayML::creator(void *pointerToAppObject)
{
    RESTNodeRelay * lRelay = new RESTNodeRelay;
    lRelay->mApp = reinterpret_cast<App *>(pointerToAppObject);
    return lRelay;
}

bool RESTNodeRelayML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool RESTNodeRelayML::startInOwnThread()
{ return false; }

bool RESTNodeRelayML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    RESTNodeRelay * lRelay = reinterpret_cast<RESTNodeRelay *>(pointerToThis);
    lRelay->mNAM = new QNetworkAccessManager;
    return true;
}

RESTNodeRelay::RESTNodeRelay()
{ }

RESTNodeRelay::~RESTNodeRelay()
{ mNAM->deleteLater(); }

QNetworkReply *RESTNodeRelay::get(const QString iCryptoTypeShortName, const QString iUrlPath)
{

}

QNetworkReply *RESTNodeRelay::post(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iPayloadType, const QByteArray iPayload)
{

}
