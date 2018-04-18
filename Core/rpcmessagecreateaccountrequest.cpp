#include "rpcmessagecreateaccountrequest.h"

RPCMessageCreateAccountRequest::RPCMessageCreateAccountRequest() : RPCMessage()
{ }

RPCMessageCreateAccountRequest::RPCMessageCreateAccountRequest(const RPCMessage &iOther)
    : RPCMessage(iOther)
{ }

RPCMessageCreateAccountRequest::RPCMessageCreateAccountRequest(const QString iMessage)
    : RPCMessage(iMessage)
{ }

QString RPCMessageCreateAccountRequest::commandValue()
{ return QStringLiteral("createAccount"); }

QString RPCMessageCreateAccountRequest::publicKeyKey()
{ return QStringLiteral("publicKey"); }

QByteArray RPCMessageCreateAccountRequest::publicKey() const
{ return QByteArray::fromBase64(fieldValue(publicKeyKey()).toByteArray()); }

QString RPCMessageCreateAccountRequest::create(const QByteArray iPublicKey, const QString iUsername, const QByteArray iPrivateKey, RPCMessage::KeyEncoding iKeyEncoding)
{ return RPCMessage::toMessage( QList<RPCField>() << RPCField{publicKeyKey(), iPublicKey.toBase64()} << RPCField{QStringLiteral(kFieldKey_Command), commandValue()}, iUsername, iPrivateKey, iKeyEncoding ); }
