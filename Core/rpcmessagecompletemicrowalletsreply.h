#ifndef RPCMESSAGECOMPLETEMICROWALLETSREPLY_H
#define RPCMESSAGECOMPLETEMICROWALLETSREPLY_H

#include "rpcmessage.h"

class RPCMessageCompleteMicroWalletsReply : public RPCMessage
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

    RPCMessageCompleteMicroWalletsReply();
    RPCMessageCompleteMicroWalletsReply(const RPCMessage &iOther);
    RPCMessageCompleteMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletPartialKeysKey();

    ReplyCode replyCode() const;
    QString transactionId() const;
    QMap< QString, QByteArray > walletPartialKeys() const;

    static QString create(
            const ReplyCode iReplyCode, const QMap< QString, QByteArray > iWalletPartialKeys, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECOMPLETEMICROWALLETSREPLY_H
