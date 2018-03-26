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
        QVariantMap     lMap = QJsonDocument::fromJson(iRequest.getBody(), &lJSonError).toVariant().toMap();
        if( lJsonError.error == QJsonParseError::NoError ) {
            if( lMap.contains(QStringLiteral("address")) && lMap.contains("publicKey") ) {

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
