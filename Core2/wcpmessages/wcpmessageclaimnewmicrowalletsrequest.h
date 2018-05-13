#ifndef WCPMESSAGECLAIMNEWMICROWALLETSREQUEST_H
#define WCPMESSAGECLAIMNEWMICROWALLETSREQUEST_H

#include "wcpmessage.h"

class WCPMessageClaimNewMicroWalletsRequest : public WCPMessage
{
public:    
    WCPMessageClaimNewMicroWalletsRequest();
    WCPMessageClaimNewMicroWalletsRequest(const WCPMessage &iOther);
    WCPMessageClaimNewMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletIdsKey();

    QString transactionId() const;
    QStringList walletIds() const;

    static QString create(
            const QStringList iWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECLAIMNEWMICROWALLETSREQUEST_H
