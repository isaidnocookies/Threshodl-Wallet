#ifndef RESTNODERELAYINTERFACE_H
#define RESTNODERELAYINTERFACE_H

#include <QNetworkRequest>
#include <QNetworkReply>

class RESTNodeRelayInterfaceInterface
{
public:
    virtual ~RESTNodeRelayInterfaceInterface()
    { }

    virtual QNetworkReply * get(const QString iCryptoTypeShortName, const QString iUrlPath)                                                         = 0;
    virtual QNetworkReply * post(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iPayloadType, const QByteArray iPayload) = 0;
};

#endif // RESTNODERELAYINTERFACE_H
