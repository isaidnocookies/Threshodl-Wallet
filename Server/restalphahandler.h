#ifndef RESTALPHAHANDLER_H
#define RESTALPHAHANDLER_H

#include "resthandler.h"

class RestAlphaHandler
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest& iRequest, HttpResponse& iResponse);
};

#endif // RESTALPHAHANDLER_H
