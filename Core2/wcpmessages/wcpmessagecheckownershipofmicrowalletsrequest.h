#ifndef WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H
#define WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H

#include "wcpmessage.h"

class WCPMessageCheckOwnershipOfMicroWalletsRequest : public WCPMessage
{
public:
    WCPMessageCheckOwnershipOfMicroWalletsRequest();
    WCPMessageCheckOwnershipOfMicroWalletsRequest(const WCPMessage &iOther);
    WCPMessageCheckOwnershipOfMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletIdsKey();
    static QString ownerKey();

    QString transactionId() const;
    QStringList walletIds() const;
    QString owner() const;

    static QString create(
            const QString iOwner, const QStringList iWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECHECKOWNERSHIPOFMICROWALLETSREQUEST_H
