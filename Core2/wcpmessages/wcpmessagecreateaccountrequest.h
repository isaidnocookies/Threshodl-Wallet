#ifndef WCPMESSAGECREATEACCOUNTREQUEST_H
#define WCPMESSAGECREATEACCOUNTREQUEST_H

#include "wcpmessage.h"

class WCPMessageCreateAccountRequest : public WCPMessage
{
public:
    WCPMessageCreateAccountRequest();
    WCPMessageCreateAccountRequest(const WCPMessage &iOther);
    WCPMessageCreateAccountRequest(const QString iMessage);

    static QString commandValue();
    static QString publicKeyKey();

    QByteArray publicKey() const;

    static QString create(const QByteArray iPublicKey, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // WCPMESSAGECREATEACCOUNTREQUEST_H
