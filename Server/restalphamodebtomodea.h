#ifndef RESTALPHAMODEBTOMODEA_H
#define RESTALPHAMODEBTOMODEA_H

#include "restalphahandler.h"

class RESTAlphaModeBToModeA
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHAMODEBTOMODEA_H
