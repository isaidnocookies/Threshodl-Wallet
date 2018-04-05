#include "rpcmessagecreateaccountreply.h"

RPCMessageCreateAccountReply::RPCMessageCreateAccountReply() : RPCMessage()
{ }

RPCMessageCreateAccountReply::RPCMessageCreateAccountReply(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageCreateAccountReply::RPCMessageCreateAccountReply(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageCreateAccountReply::commandValue()
{ return QStringLiteral("createAccount"); }

QString RPCMessageCreateAccountReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

RPCMessageCreateAccountReply::ReplyCode RPCMessageCreateAccountReply::replyCode() const
{ return static_cast<RPCMessageCreateAccountReply::ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString RPCMessageCreateAccountReply::create(const RPCMessageCreateAccountReply::ReplyCode iReplyCode, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{ return RPCMessage::toMessage( QList<RPCField>() << RPCField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)} << RPCField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, iPrivateKey, iKeyEncoding ); }
