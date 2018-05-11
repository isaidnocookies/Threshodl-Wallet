#ifndef WCPMESSAGEPINGREPLY_H
#define WCPMESSAGEPINGREPLY_H

#include "wcpmessage.h"

class WCPMessagePingReply : public WCPMessage
{
public:
    WCPMessagePingReply();
    WCPMessagePingReply(const WCPMessage &iOther);
    WCPMessagePingReply(const QString iMessage);

    static QString commandValue();
    static QString pingPayloadKey();

    QByteArray payload() const;

    static QString create(const QByteArray iPayload, const QString iUsername);
};

#endif // WCPMESSAGEPINGREPLY_H
