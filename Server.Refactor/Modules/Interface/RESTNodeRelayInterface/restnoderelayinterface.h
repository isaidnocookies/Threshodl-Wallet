#ifndef RESTNODERELAYINTERFACE_H
#define RESTNODERELAYINTERFACE_H

#include <QNetworkRequest>
#include <QNetworkReply>

class RESTNodeRelayInterfaceInterface
{
public:
    virtual ~RESTNodeRelayInterfaceInterface()
    { }

    virtual QNetworkReply * relayAndWaitForResponse(QNetworkRequest &iRequest, int iTimeOutInMS = 30000) = 0;
};

#endif // RESTNODERELAYINTERFACE_H
