#ifndef RESTHANDLER_H
#define RESTHANDLER_H

#include "httprequesthandler.h"

class RESTHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    RESTHandler( QObject * iParent = nullptr );
    ~RESTHandler();

    /**
        Handles HTTP Requests
        @param request The received HTTP request
        @param response Must be used to return the response
    */
    void service(HttpRequest& iRequest, HttpResponse& iResponse) override;

    bool serviceVersionAlpha(const QByteArray & iPathBalance, HttpRequest& iRequest, HttpResponse& iResponse);
};

#endif // RESTHANDLER_H
