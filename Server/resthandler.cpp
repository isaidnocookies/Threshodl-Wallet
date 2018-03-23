#include "resthandler.h"
#include "restalphaexchangerate.h"

RESTHandler::RESTHandler(QObject *iParent)
    : HttpRequestHandler(iParent)
{

}

RESTHandler::~RESTHandler()
{

}

void RESTHandler::service(HttpRequest &iRequest, HttpResponse &iResponse)
{
    bool            lInvalidRequest     = true;
    QByteArray      lPath               = iRequest.getPath().replace("//","/");
    int             lOffset             = lPath.indexOf('/',1);

    if( lOffset > -1 ) {
        QByteArray  lStarts = lPath.left(lOffset);
        if( lStarts == "/a" )
            lInvalidRequest = serviceVersionAlpha( lPath.mid(lOffset), iRequest, iResponse );
    }

    if( lInvalidRequest ) {
        iResponse.setHeader("Content-Type", "text/html; charset=ISO-8859-1");
        iResponse.write("<html><head>"
                       "<meta HTTP-EQUIV=\"REFRESH\" content=\"0; url=http://www.threshodl.com\">"
                       "</head><body>"
                       "Redirecting to <a href=\"http://www.aquaconnect.net\">www.threshodl.com</a>"
                       "</body></html>",true);
    }
}

bool RESTHandler::serviceVersionAlpha(const QByteArray &iPathBalance, HttpRequest &iRequest, HttpResponse &iResponse)
{
    int         lOffset         = iPathBalance.indexOf('/',1);
    QByteArray  lCommand;
    QByteArray  lPathBalance    = iPathBalance;

    if( lOffset > -1 ) {
        lCommand = lPathBalance.mid(1,lOffset-1);
        lPathBalance = lPathBalance.mid(lOffset);

        if( lCommand == "exchangerate" ) {
            return RESTAlphaExchangeRate::service(lPathBalance,iRequest,iResponse);
        }
    }

    return false;
}
