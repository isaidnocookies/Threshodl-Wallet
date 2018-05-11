#ifndef WCPMESSAGECOMPLETEMICROWALLETSREPLY_H
#define WCPMESSAGECOMPLETEMICROWALLETSREPLY_H

#include "wcpmessage.h"

class WCPMessageCompleteMicroWalletsReply : public WCPMessage
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

    WCPMessageCompleteMicroWalletsReply();
    WCPMessageCompleteMicroWalletsReply(const WCPMessage &iOther);
    WCPMessageCompleteMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();              // Untouched by the server
    static QString walletPartialKeysKey();

    ReplyCode replyCode() const;
    QString transactionId() const;
    QMap< QString, QByteArray > walletPartialKeys() const;

    static QString create(
            const ReplyCode iReplyCode, const QMap< QString, QByteArray > iWalletPartialKeys, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGECOMPLETEMICROWALLETSREPLY_H