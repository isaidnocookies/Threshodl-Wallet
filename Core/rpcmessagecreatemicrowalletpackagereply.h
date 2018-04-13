#ifndef RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H
#define RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H

#include "rpcmessage.h"

class RPCMessageCreateMicroWalletPackageReply : public RPCMessage
{
public:
    RPCMessageCreateMicroWalletPackageReply();
    RPCMessageCreateMicroWalletPackageReply(const RPCMessage &iOther);
    RPCMessageCreateMicroWalletPackageReply(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString microWalletsDataKey();

    QString transactionId() const;
    QList<QByteArray> microWalletsData() const;

    static QString create(
            const QList<QByteArray> iMicroWalletDatas, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECREATEMICROWALLETPACKAGEREPLY_H
