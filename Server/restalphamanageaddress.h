#ifndef RESTALPHACREATEADDRESS_H
#define RESTALPHACREATEADDRESS_H

#include "restalphahandler.h"

class RESTAlphaManageAddress
{
public:
    static bool serviceCreate(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
    static bool serviceDelete(const QByteArray &iRESTArguments, HttpRequest &iRequest, HttpResponse &iResponse);
};

#endif // RESTALPHACREATEADDRESS_H
