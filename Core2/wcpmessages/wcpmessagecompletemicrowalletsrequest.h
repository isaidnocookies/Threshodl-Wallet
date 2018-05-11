#ifndef WCPMESSAGECOMPLETEMICROWALLETSREQUEST_H
#define WCPMESSAGECOMPLETEMICROWALLETSREQUEST_H

#include "wcpmessage.h"

class WCPMessageCompleteMicroWalletsRequest : public WCPMessage
{
public:
    WCPMessageCompleteMicroWalletsRequest();
    WCPMessageCompleteMicroWalletsRequest(const WCPMessage &iOther);
    WCPMessageCompleteMicroWalletsRequest(const QString iMessage);

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

#endif // WCPMESSAGECOMPLETEMICROWALLETSREQUEST_H
