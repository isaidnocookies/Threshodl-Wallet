#ifndef RPCMESSAGECREATEACCOUNTREPLY_H
#define RPCMESSAGECREATEACCOUNTREPLY_H

#include "rpcmessage.h"

class RPCMessageCreateAccountReply : public RPCMessage
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

    RPCMessageCreateAccountReply();
    RPCMessageCreateAccountReply(const RPCMessage &iOther);
    RPCMessageCreateAccountReply(const QString iMessage);

    static QString commandValue();
    static QString replyCodeKey();
    static QString replyUsernameKey();

    ReplyCode replyCode() const;
    QString replyUsername() const;

    static QString create(const ReplyCode iReplyCode, const QString iReplyUsername, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding = KeyEncoding::SHA512);
};

#endif // RPCMESSAGECREATEACCOUNTREPLY_H
