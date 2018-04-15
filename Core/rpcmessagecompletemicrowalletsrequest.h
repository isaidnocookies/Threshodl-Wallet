#ifndef RPCMESSAGECOMPLETEMICROWALLETSREQUEST_H
#define RPCMESSAGECOMPLETEMICROWALLETSREQUEST_H

#include "rpcmessage.h"

class RPCMessageCompleteMicroWalletsRequest : public RPCMessage
{
public:
    RPCMessageCompleteMicroWalletsRequest();
    RPCMessageCompleteMicroWalletsRequest(const RPCMessage &iOther);
    RPCMessageCompleteMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletIdsKey();

    QString transactionId() const;
    QStringList walletIds() const;

    static QString create(
            const QStringList iWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGECOMPLETEMICROWALLETSREQUEST_H
