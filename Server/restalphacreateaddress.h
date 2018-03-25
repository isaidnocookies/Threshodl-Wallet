#ifndef RESTALPHACREATEADDRESS_H
#define RESTALPHACREATEADDRESS_H

#include "restalphahandler.h"

class RESTAlphaCreateAddress
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHACREATEADDRESS_H
