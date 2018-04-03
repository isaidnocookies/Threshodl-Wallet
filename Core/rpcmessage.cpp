#include "rpcmessage.h"
#include "digest.h"

#include <QVariantMap>
#include <QJsonDocument>

#define     kUsernameKey            "user"
#define     kSignatureKey           "sig"
#define     kSignatureEncodingKey   "sige"
#define     kInnerMapKey            "data"

RPCMessage::RPCMessage(const RPCMessage &iOther)
{
    _copy(iOther);
}

RPCMessage::RPCMessage(const QString iMessage)
{
    QByteArray          lUtf8Message    = iMessage.toUtf8();
    QJsonParseError     lJsonParseError;

    QVariantMap lOutterMap = QJsonDocument::fromJson(lUtf8Message,&lJsonParseError).toVariant().toMap();
    if( lJsonParseError.error == QJsonParseError::NoError ) {
        mUsername           = lOutterMap[kUsernameKey].toString();
        mSignature          = lOutterMap[kSignatureKey].toByteArray();
        mSignatureEncoding  = lOutterMap[kSignatureEncodingKey].toInt();

        QVariantMap lInnerMap = lOutterMap[kInnerMapKey];
        for( auto lKey : lInnerMap.keys() ) {
            mFields << RPCField{lKey, lInnerMap[lKey]};
        }
    }
}

RPCMessage &RPCMessage::operator=(const RPCMessage &iOther)
{ _copy(iOther); }

RPCMessage &RPCMessage::operator<<(const RPCField &iField)
{ mFields << iField; }

QString RPCMessage::toMessage(const QString iUsername, const QByteArray iPublicKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    mUsername = iUsername;
    mSignature = Digest::sign(iPublicKey,lData,lHashType);
}

QString RPCMessage::username() const
{ return mUsername; }

QByteArray RPCMessage::signature() const
{ return mSignature; }

RPCMessage::KeyEncoding RPCMessage::signatureKeyEncoding() const
{ return static_cast<RPCMessage::KeyEncoding>(mSignatureEncoding); }

QList<RPCField> RPCMessage::fields() const
{ return mFields; }
