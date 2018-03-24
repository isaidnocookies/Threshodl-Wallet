#ifndef RESTALPHAEXCHANGERATE_H
#define RESTALPHAEXCHANGERATE_H

#include "restalphahandler.h"

class RESTAlphaExchangeRate
{
public:
    /*
     * iRESTArguments format:
     * /<from>/<to>
     *
     * iRESTArguments examples:
     * /btc/usd
     * /eth/usd
     * /eth/btc
     *
     */
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHAEXCHANGERATE_H
