#ifndef RESTALPHAMODEATOMODEB_H
#define RESTALPHAMODEATOMODEB_H

#include "restalphahandler.h"

class RESTAlphaModeAToModeB
{
public:
    static bool service(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHAMODEATOMODEB_H
