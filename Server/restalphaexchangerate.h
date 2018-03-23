#ifndef RESTALPHAEXCHANGERATE_H
#define RESTALPHAEXCHANGERATE_H

#include "httprequesthandler.h"

class RESTAlphaExchangeRate
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHAEXCHANGERATE_H
