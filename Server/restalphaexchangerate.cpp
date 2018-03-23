#include "restalphaexchangerate.h"
#include "main.h"
#include "app.h"

bool RESTAlphaExchangeRate::service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse)
{
    Q_UNUSED(iRequest)

    QList<QByteArray>   lElements   = iRESTArguments.split('/');
    QByteArray          lReply;

    if( lElements.size() == 3 ) {
        QByteArray      lFrom       = lElements[1];
        QByteArray      lTo         = lElements[2];

        if( lFrom == "btc" ) {
            if( lTo == "usd" ) {
                lReply = gApp->recordsManager()->lastDataBTCUSD();
            }
        } else if( lFrom == "eth" ) {
            if( lTo == "usd" ) {
                lReply = gApp->recordsManager()->lastDataETHUSD();
            } else if( lFrom == "btc" ) {
                lReply = gApp->recordsManager()->lastDataETHBTC();
            }
        }
    }

    if( ! lReply.isEmpty() ) {
        iResponse.setHeader("Content-Type","application/json");
        iResponse.write(lReply);
        return true;
    }

    return false;
}
