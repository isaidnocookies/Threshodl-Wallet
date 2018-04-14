#ifndef RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H
#define RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H

#include "rpcmessage.h"

class RPCMessageCheckOwnershipOfMicroWalletsRequest : public RPCMessage
{
public:
    RPCMessageCheckOwnershipOfMicroWalletsRequest();
    RPCMessageCheckOwnershipOfMicroWalletsRequest(const RPCMessage &iOther);
    RPCMessageCheckOwnershipOfMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletIdsKey();
    static QString ownerKey();

    QString transactionId() const;
    QStringList walletIds() const;
    QString owner() const;

    static QString create(
            const QString iOwner, const QStringList iWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H
