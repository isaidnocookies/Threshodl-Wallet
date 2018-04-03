#include "rpcmessage.h"
#include "digest.h"

#include <QVariantMap>
#include <QJsonDocument>

#define     kUsernameKey            "user"
#define     kSignatureKey           "sig"
#define     kSignatureEncodingKey   "sige"
#define     kInnerMapKey            "data"

inline static QVariantMap __fieldsToMap(QList<RPCField> &iFields)
{
    QVariantMap lMap;

    for( auto &lField : iFields ) {
        lMap[lField.Key] = lField.Value;
    }

    return lMap;
}

RPCMessage::RPCMessage(const RPCMessage &iOther)
{ _copy(iOther); }

RPCMessage::RPCMessage(const QString iMessage)
{
    QByteArray          lUtf8Message    = iMessage.toUtf8();
    QJsonParseError     lJsonParseError;

    QVariantMap lOutterMap = QJsonDocument::fromJson(lUtf8Message,&lJsonParseError).toVariant().toMap();
    if( lJsonParseError.error == QJsonParseError::NoError ) {
        mUsername           = lOutterMap[kUsernameKey].toString();
        mSignature          = lOutterMap[kSignatureKey].toByteArray();
        mSignatureEncoding  = lOutterMap[kSignatureEncodingKey].toInt();

        QByteArray  lData       = lOutterMap[kInnerMapKey].toByteArray();
        QVariantMap lInnerMap   = QJsonDocument::fromJson(lData,&lJsonParseError).toVariant().toMap();
        if( lJsonParseError.error == QJsonParseError::NoError ) {
            for( auto lKey : lInnerMap.keys() ) {
                mFields << RPCField{lKey, lInnerMap[lKey]};
            }
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

    QVariantMap lInnerMap       = __fieldsToMap(fields());
    QByteArray  lData           = QJsonDocument::fromVariant(lInnerMap).toJson();
    QVariantMap lOutterMap;

    enum Digest::HashTypes lHashType;
    switch( iKeyEncoding ) {

    case RPCMessage::KeyEncoding::SHA512:
    default:
        lHashType = Digest::HashTypes::SHA512;
        break;
    }

    mSignature = Digest::sign(iPublicKey,lData,lHashType);

    lOutterMap[kUsernameKey]            = mUsername;
    lOutterMap[kSignatureKey]           = mSignature;
}

QString RPCMessage::username() const
{ return mUsername; }

QByteArray RPCMessage::signature() const
{ return mSignature; }

RPCMessage::KeyEncoding RPCMessage::signatureKeyEncoding() const
{ return static_cast<RPCMessage::KeyEncoding>(mSignatureEncoding); }

QList<RPCField> RPCMessage::fields() const
{ return mFields; }
