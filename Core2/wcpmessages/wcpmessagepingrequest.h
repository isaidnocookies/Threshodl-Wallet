#ifndef WCPMESSAGEPINGREQUEST_H
#define WCPMESSAGEPINGREQUEST_H

#include "wcpmessage.h"

class WCPMessagePingRequest : public WCPMessage
{
public:
    WCPMessagePingRequest();
    WCPMessagePingRequest(const WCPMessage &iOther);
    WCPMessagePingRequest(const QString iMessage);

    static QString commandValue();
    static QString payloadKey();

    QByteArray payload() const;

    static QString create(const QByteArray iPayload, const QString iUsername);
};

#endif // WCPMESSAGEPINGREQUEST_H
