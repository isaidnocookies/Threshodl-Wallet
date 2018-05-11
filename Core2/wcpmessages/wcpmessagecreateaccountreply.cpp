#include "wcpmessagecreateaccountreply.h"

WCPMessageCreateAccountReply::WCPMessageCreateAccountReply() : WCPMessage()
{ }

WCPMessageCreateAccountReply::WCPMessageCreateAccountReply(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageCreateAccountReply::WCPMessageCreateAccountReply(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageCreateAccountReply::commandValue()
{ return QStringLiteral("createAccount"); }

QString WCPMessageCreateAccountReply::replyCodeKey()
{ return QStringLiteral("replyCode"); }

QString WCPMessageCreateAccountReply::replyUsernameKey()
{ return QStringLiteral("replyUsername"); }

WCPMessageCreateAccountReply::ReplyCode WCPMessageCreateAccountReply::replyCode() const
{ return static_cast<WCPMessageCreateAccountReply::ReplyCode>(fieldValue(replyCodeKey()).toUInt()); }

QString WCPMessageCreateAccountReply::replyUsername() const
{ return fieldValue(replyUsernameKey()).toString(); }

QString WCPMessageCreateAccountReply::create(const WCPMessageCreateAccountReply::ReplyCode iReplyCode, const QString iReplyUsername, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    return WCPMessage::toMessage(
                QList<WCPField>()
                << WCPField{replyCodeKey(), static_cast<unsigned int>(iReplyCode)}
                << WCPField{replyUsernameKey(), iReplyUsername}
                << WCPField{QStringLiteral(kFieldKey_Command), commandValue()},
                iUsername, iPrivateKey, iKeyEncoding );
}
