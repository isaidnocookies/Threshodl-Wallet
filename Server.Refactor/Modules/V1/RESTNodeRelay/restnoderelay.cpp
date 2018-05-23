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
    lRelay->loadConfigurationValues();
    return true;
}

void RESTNodeRelay::loadConfigurationValues()
{
    if( ! mApp ) return;

    auto lConf = mApp->configuration();

    if( lConf->contains(QStringLiteral("RESTNodeRelay")) ) {
        for( QVariant lEntry : lConf->toVariantList(QStringLiteral("RESTNodeRelay")) )
        {
            QVariantMap lVM = lEntry.toMap();
            if( lVM.contains(QStringLiteral("cryptoTypeShortName")) && lVM.contains(QStringLiteral("urlBase")) )
            {
                mCryptoToUrlBase[lVM[QStringLiteral("cryptoTypeShortName")].toString().toLower()] = lVM[QStringLiteral("urlBase")].toString();
            }
        }
    }
}

RESTNodeRelay::RESTNodeRelay()
{ }

RESTNodeRelay::~RESTNodeRelay()
{ mNAM->deleteLater(); }

QNetworkReply *RESTNodeRelay::get(const QString iCryptoTypeShortName, const QString iUrlPath)
{
    QString lBase       = mCryptoToUrlBase[iCryptoTypeShortName.toLower()];
    QString lUrlString;

    if( lBase.endsWith(QChar{'/'}) && iUrlPath.startsWith(QChar{'/'}) ) {
        lUrlString = QStringLiteral("%1%2").arg(lBase).arg(iUrlPath.mid(1));
    } else if( ! lBase.endsWith(QChar{'/'}) && ! iUrlPath.startsWith(QChar{'/'}) ) {
        lUrlString = QStringLiteral("%1/%2").arg(lBase).arg(iUrlPath);
    } else {
        lUrlString = QStringLiteral("%1%2").arg(lBase).arg(iUrlPath);
    }

    return mNAM->get(QNetworkRequest{QUrl{lUrlString}});
}

QNetworkReply *RESTNodeRelay::post(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iPayloadType, const QByteArray iPayload)
{
    QString lBase       = mCryptoToUrlBase[iCryptoTypeShortName.toLower()];
    QString lUrlString;

    if( lBase.endsWith(QChar{'/'}) && iUrlPath.startsWith(QChar{'/'}) ) {
        lUrlString = QStringLiteral("%1%2").arg(lBase).arg(iUrlPath.mid(1));
    } else if( ! lBase.endsWith(QChar{'/'}) && ! iUrlPath.startsWith(QChar{'/'}) ) {
        lUrlString = QStringLiteral("%1/%2").arg(lBase).arg(iUrlPath);
    } else {
        lUrlString = QStringLiteral("%1%2").arg(lBase).arg(iUrlPath);
    }

    QNetworkRequest lRequest{QUrl{lUrlString}};
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, iPayloadType);
    return mNAM->post(lRequest,iPayload);
}
