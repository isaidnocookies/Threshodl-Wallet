#ifndef RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H
#define RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H

#include "rpcmessage.h"

class RPCMessageCreateMicroWalletPackageReply : public RPCMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        Unauthorized                    = 0x2,
        UnhandledCryptoType             = 0x3,
        UnableToGrindUpCryptoCurrency   = 0x4,
        InternalServerError1            = 0xA0
    };

    typedef enum ReplyCode ReplyCode;

    RPCMessageCreateMicroWalletPackageReply();
    RPCMessageCreateMicroWalletPackageReply(const RPCMessage &iOther);
    RPCMessageCreateMicroWalletPackageReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString microWalletsDataKey();

    ReplyCode replyCode() const;
    QString transactionId() const;
    QList<QByteArray> microWalletsData() const;

    static QString create(
            const ReplyCode iReplyCode, const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H
