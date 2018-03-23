#include "restalphaexchangerate.h"
#include "main.h"
#include "app.h"

bool RESTAlphaExchangeRate::service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse)
{
    Q_UNUSED(iRequest)

    QList<QByteArray>   lElements   = iRESTArguments.split('/');
    QByteArray          lReply;

    if( lElements.size() == 3 ) {
        QByteArray      lFrom       = lElements[1].toLower();
        QByteArray      lTo         = lElements[2].toLower();

        if( lFrom == "btc" ) {
            if( lTo == "usd" ) {
                lReply = gApp->recordsManager()->lastDataBTCUSD();
            } else {
                qWarning() << "Unable to convert 'btc' to unknown type" << lTo;
            }
        } else if( lFrom == "eth" ) {
            if( lTo == "usd" ) {
                lReply = gApp->recordsManager()->lastDataETHUSD();
            } else if( lTo == "btc" ) {
                lReply = gApp->recordsManager()->lastDataETHBTC();
            } else {
                qWarning() << "Unable to convert 'eth' to unknown type" << lTo;
            }
        }
    } else {
        qWarning() << "Wrong number of element count, expecting 3, received" << lElements.size();
    }

    if( ! lReply.isEmpty() ) {
        iResponse.setHeader("Content-Type","application/json");
        iResponse.write(lReply);
        return true;
    } else {
        qWarning() << "There is no reply message for the exchange rate request.";
    }

    return false;
}
