#ifndef WCPMESSAGESENDNODERESTMESSAGEREQUEST_H
#define WCPMESSAGESENDNODERESTMESSAGEREQUEST_H

#include "wcpmessage.h"

class WCPMessageSendNodeRestMessageRequest : public WCPMessage
{
public:
    enum Method {
        Get     = 0,
        Post    = 1
    };

    typedef enum Method Method;

    WCPMessageSendNodeRestMessageRequest();
    WCPMessageSendNodeRestMessageRequest(const WCPMessage &iOther);
    WCPMessageSendNodeRestMessageRequest(const QString iMessage);

    static QString commandValue();
    static QString cryptoTypeShortNameKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString urlPathKey();
    static QString methodKey();
    static QString payloadTypeKey();
    static QString payloadKey();

    QString cryptoTypeShortName() const;
    QString transactionId() const;
    QString urlPath() const;
    Method method() const;
    QString payloadType() const;
    QByteArray payload() const;

    static QString createGet(
            const QString iCryptoTypeShortName, const QString iUrlPath, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );

    static QString createPost(
            const QString iCryptoTypeShortName, const QString iUrlPath, const QString iTransactionId,
            const QString iPayloadType, const QByteArray iPayload,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGESENDNODERESTMESSAGEREQUEST_H
