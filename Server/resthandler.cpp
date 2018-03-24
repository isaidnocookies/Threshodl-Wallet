#include "resthandler.h"
#include "restalphahandler.h"

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
            lInvalidRequest = ! RestAlphaHandler::service( lPath.mid(lOffset), iRequest, iResponse );
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

