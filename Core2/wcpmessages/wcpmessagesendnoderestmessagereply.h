#ifndef WCPMESSAGESENDNODERESTMESSAGEREPLY_H
#define WCPMESSAGESENDNODERESTMESSAGEREPLY_H

#include "wcpmessage.h"

#include <QNetworkReply>

class WCPMessageSendNodeRestMessageReply : public WCPMessage
{
public:
    WCPMessageSendNodeRestMessageReply();
    WCPMessageSendNodeRestMessageReply(const WCPMessage &iOther);
    WCPMessageSendNodeRestMessageReply(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString replyCodeKey();
    static QString payloadKey();

    QString transactionId() const;
    QNetworkReply::NetworkError replyCode() const;
    QByteArray payload() const;

    static QString create(
            const QString iTransactionId, const QByteArray iPayload, const QNetworkReply::NetworkError iReplyCode,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGESENDNODERESTMESSAGEREPLY_H
