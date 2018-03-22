#include "resthandler.h"

#include <QDebug>

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
    QByteArray      lPath               = iRequest.getPath();

    qWarning() << "Path:" << lPath.constData();

    if( lPath.startsWith("/a/exchangerate/") ) {
        if( lPath == "/a/exchangerate/btc/usd" ) {
            lInvalidRequest = false;
        } else if( lPath == "/a/exchangerate/eth/usd" ) {
            lInvalidRequest = false;
        } else if( lPath == "/a/exchangerate/eth/btc" ) {
            lInvalidRequest = false;
        }
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
