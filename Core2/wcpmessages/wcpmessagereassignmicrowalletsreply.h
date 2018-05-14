#ifndef WCPMESSAGEREASSIGNMICROWALLETSREPLY_H
#define WCPMESSAGEREASSIGNMICROWALLETSREPLY_H

#include "wcpmessage.h"

class WCPMessageReassignMicroWalletsReply : public WCPMessage
{
public:
    enum ReplyCode {
        UnknownFailure                  = 0x0,
        Success                         = 0x1,
        SourceDoesNotExist              = 0x2,
        DestinationDoesNotExist         = 0x3,
        OneOrMoreWalletsUnauthorized    = 0x4,
        OneOrMoreWalletsDoNotExist      = 0x5,
        Failure                         = 0x6,
        InternalServerError1            = 0xA0,
        InternalServerError2            = 0xA1,
        InternalServerError3            = 0xA2
    };

    typedef enum ReplyCode ReplyCode;

    WCPMessageReassignMicroWalletsReply();
    WCPMessageReassignMicroWalletsReply(const WCPMessage &iOther);
    WCPMessageReassignMicroWalletsReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString transactionIdKey();

    ReplyCode replyCode() const;
    QString transactionId() const;      // Custom data passed back from the request untouched

    static QString create(
            const ReplyCode iReplyCode, const QString iTransactionId,
            const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512
            );
};

#endif // WCPMESSAGEREASSIGNMICROWALLETSREPLY_H
