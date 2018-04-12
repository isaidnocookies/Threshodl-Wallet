#ifndef RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H
#define RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H

#include "rpcmessage.h"

class RPCMessageReassignMicroWalletsRequest : public RPCMessage
{
public:
    RPCMessageReassignMicroWalletsRequest();
    RPCMessageReassignMicroWalletsRequest(const RPCMessage &iOther);
    RPCMessageReassignMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString destinationKey();
    static QString microWalletIdsKey();
    static QString transactionIdKey();

    QString destination() const;
    QStringList microWalletIds() const;
    QString transactionId() const;          // Custom data passed back on the reply untouched

    static QString create(
            const QString iDestination, const QStringList iMicroWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // RPCMESSAGEREASSIGNMICROWALLETSREQUEST_H
