#ifndef WCPMESSAGECREATEACCOUNTREPLY_H
#define WCPMESSAGECREATEACCOUNTREPLY_H

#include "wcpmessage.h"

class WCPMessageCreateAccountReply : public WCPMessage
{
public:
    enum ReplyCode {
        UnknownFailure          = 0x0,
        Success                 = 0x1,
        UsernameTaken           = 0x2,
        KeyInvalid              = 0x3,
        KeyTypeInvalid          = 0x4,
        InternalServerError1    = 0xA0,
    };

    typedef enum ReplyCode ReplyCode;

    WCPMessageCreateAccountReply();
    WCPMessageCreateAccountReply(const WCPMessage &iOther);
    WCPMessageCreateAccountReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString replyUsernameKey();

    ReplyCode replyCode() const;
    QString replyUsername() const;

    static QString create(const ReplyCode iReplyCode, const QString iReplyUsername, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // WCPMESSAGECREATEACCOUNTREPLY_H
