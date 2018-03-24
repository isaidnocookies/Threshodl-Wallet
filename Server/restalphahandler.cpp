#include "restalphahandler.h"
#include "restalphaexchangerate.h"
#include "restalphamodeatomodeb.h"

#include <QDebug>

bool RestAlphaHandler::service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse)
{
    QByteArray  lCommand;
    int         lOffset         = iRESTArguments.indexOf('/',1);
    QByteArray  lRESTArguments    = iRESTArguments;

    if( lOffset > -1 ) {
        lCommand = lRESTArguments.mid( 1, lOffset-1 );
        lRESTArguments = lRESTArguments.mid( lOffset );

        if( lCommand == "exchangerate" )
            return RESTAlphaExchangeRate::service( lRESTArguments, iRequest, iResponse );

        if( lCommand == "atob" )
            return RestAlphaModeAToModeB::service( lRESTArguments, iRequest, iResponse );

        qWarning() << "Unknown command for 'alpha'" << lCommand;
    } else {
        qWarning() << "Failed to parse 'alpha' rest path" << iRESTArguments;
    }

    return false;
}
