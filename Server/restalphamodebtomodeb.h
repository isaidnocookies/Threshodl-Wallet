#ifndef RESTALPHAMODEBTOMODEB_H
#define RESTALPHAMODEBTOMODEB_H

#include "restalphahandler.h"

class RESTAlphaModeBToModeB
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHAMODEBTOMODEB_H
