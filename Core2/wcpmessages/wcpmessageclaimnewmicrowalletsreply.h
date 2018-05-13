#ifndef WCPMESSAGECLAIMNEWMICROWALLETSREPLY_H
#define WCPMESSAGECLAIMNEWMICROWALLETSREPLY_H

#include "wcpmessage.h"

class WCPMessageClaimNewMicroWalletsReply : public WCPMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        OneOrMoreMicroWalletsDoNotExist = 0x2,
        OneOrMoreUnauthorized           = 0x3,
        InternalServerError1            = 0xA0,
        InternalServerError2            = 0xA1,
        InternalServerError3            = 0xA2
    };

    typedef enum ReplyCode ReplyCode;

    WCPMessageClaimNewMicroWalletsReply();
    WCPMessageClaimNewMicroWalletsReply(const WCPMessage &iOther);
    WCPMessageClaimNewMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString claimedWalletIdsKey();

    ReplyCode replyCode() const;
    QString transactionId() const;
    QStringList claimedWalletIds() const;

    static QString create(const ReplyCode iReplyCode, const QStringList iClaimedWalletIds, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECLAIMNEWMICROWALLETSREPLY_H
