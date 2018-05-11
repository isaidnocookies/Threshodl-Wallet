#ifndef WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H
#define WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H

#include "wcpmessage.h"

class WCPMessageCheckOwnershipOfMicroWalletsReply : public WCPMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        OneOrMoreMicroWalletsDoNotExist = 0x2,
        OneOrMoreUnauthorized           = 0x3,
        InternalServerError1            = 0xA0
    };

    typedef enum ReplyCode ReplyCode;

    WCPMessageCheckOwnershipOfMicroWalletsReply();
    WCPMessageCheckOwnershipOfMicroWalletsReply(const WCPMessage &iOther);
    WCPMessageCheckOwnershipOfMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server

    ReplyCode replyCode() const;
    QString transactionId() const;

    static QString create(
            const ReplyCode iReplyCode, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H
