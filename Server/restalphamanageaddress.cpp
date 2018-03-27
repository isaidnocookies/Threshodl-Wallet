#include "restalphamanageaddress.h"
#include "app.h"

#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>

bool RESTAlphaManageAddress::serviceCreate(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse)
{
    Q_UNUSED(iRESTArguments)

    if( gApp && gApp->databaseInterface() ) {
        QJsonParseError lJsonError;
        QVariantMap     lMap = QJsonDocument::fromJson(iRequest.getBody(), &lJsonError).toVariant().toMap();
        if( lJsonError.error == QJsonParseError::NoError ) {
            if( lMap.contains(QStringLiteral("address")) && lMap.contains(QStringLiteral("publicKey")) ) {

                QVariantMap lResponse;
                lResponse[QStringLiteral("created")] = 0;

                if( gApp->databaseInterface()->addressCreate(lMap[QStringLiteral("address")].toString(), lMap[QStringLiteral("publicKey")].toByteArray()) ) {
                    lResponse[QStringLiteral("created")] = 1;
                } else {
                    qInfo() << "Failed to create account for" << lMap[QStringLiteral("address")].toString();
                }

                iResponse.setHeader("Content-Type", "application/json");
                iResponse.write( QJsonDocument::fromVariant(lResponse).toJson() );
                return true;

            } else {
                qWarning() << "Received an invalid create address request, corrupt message that is missing one or more invalid JSON fields.";
            }
        }else{
            qWarning() << "Received an invalid create address request, corrupt message that did not parse into a JSON message.";
        }
    }

    return false;
}

bool RESTAlphaManageAddress::serviceDelete(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse)
{
    Q_UNUSED(iRequest)

    return false;
}
