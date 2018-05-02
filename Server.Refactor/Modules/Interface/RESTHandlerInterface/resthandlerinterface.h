#ifndef RESTHANDLERINTERFACE_H
#define RESTHANDLERINTERFACE_H

#include "httprequesthandler.h"

class RESTHandlerInterface : public HttpRequestHandler
{
    Q_OBJECT
public:
    RESTHandlerInterface( QObject * iParent = nullptr )
        : HttpRequestHandler(iParent)
    { }

    virtual ~RESTHandlerInterface()
    { }

    /**
        Handles HTTP Requests
        @param request The received HTTP request
        @param response Must be used to return the response
    */
    void service(HttpRequest& iRequest, HttpResponse& iResponse) override
    { HttpRequestHandler::service(iRequest,iResponse); }
};

#endif // RESTHANDLERINTERFACE_H
