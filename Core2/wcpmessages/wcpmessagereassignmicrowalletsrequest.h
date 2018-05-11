#ifndef WCPMESSAGEREASSIGNMICROWALLETSREQUEST_H
#define WCPMESSAGEREASSIGNMICROWALLETSREQUEST_H

#include "wcpmessage.h"

class WCPMessageReassignMicroWalletsRequest : public WCPMessage
{
public:
    WCPMessageReassignMicroWalletsRequest();
    WCPMessageReassignMicroWalletsRequest(const WCPMessage &iOther);
    WCPMessageReassignMicroWalletsRequest(const QString iMessage);

    static QString commandValue();
    static QString destinationKey();
    static QString microWalletIdsKey();
    static QString transactionIdKey();

    QString destination() const;
    QStringList microWalletIds() const;
    QString transactionId() const;          // Custom data passed back on the reply untouched

    static QString create(
            const QString iDestination, const QStringList iMicroWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGEREASSIGNMICROWALLETSREQUEST_H
