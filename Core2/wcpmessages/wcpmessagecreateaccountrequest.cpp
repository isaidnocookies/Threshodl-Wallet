#include "wcpmessagecreateaccountrequest.h"

WCPMessageCreateAccountRequest::WCPMessageCreateAccountRequest() : WCPMessage()
{ }

WCPMessageCreateAccountRequest::WCPMessageCreateAccountRequest(const WCPMessage &iOther)
    : WCPMessage(iOther)
{ }

WCPMessageCreateAccountRequest::WCPMessageCreateAccountRequest(const QString iMessage)
    : WCPMessage(iMessage)
{ }

QString WCPMessageCreateAccountRequest::commandValue()
{ return QStringLiteral("createAccount"); }

QString WCPMessageCreateAccountRequest::publicKeyKey()
{ return QStringLiteral("publicKey"); }

QByteArray WCPMessageCreateAccountRequest::publicKey() const
{ return QByteArray::fromBase64(fieldValue(publicKeyKey()).toByteArray()); }

QString WCPMessageCreateAccountRequest::create(const QByteArray iPublicKey, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{ return WCPMessage::toMessage( QList<WCPField>() << WCPField{publicKeyKey(), iPublicKey.toBase64()} << WCPField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, iPrivateKey, iKeyEncoding ); }
