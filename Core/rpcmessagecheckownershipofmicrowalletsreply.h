#ifndef RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H
#define RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H

#include "rpcmessage.h"

class RPCMessageCheckOwnershipOfMicroWalletsReply : public RPCMessage
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

    RPCMessageCheckOwnershipOfMicroWalletsReply();
    RPCMessageCheckOwnershipOfMicroWalletsReply(const RPCMessage &iOther);
    RPCMessageCheckOwnershipOfMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server

    ReplyCode replyCode() const;
    QString transactionId() const;

    static QString create(
            const ReplyCode iReplyCode, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREPLY_H
