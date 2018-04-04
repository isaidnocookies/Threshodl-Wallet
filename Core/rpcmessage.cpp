#include "rpcmessage.h"
#include "digest.h"

#include <QDebug>
#include <QVariantMap>
#include <QJsonDocument>

#define     kUsernameKey            "user"
#define     kSignatureKey           "sig"
#define     kSignatureEncodingKey   "sige"
#define     kInnerMapKey            "data"

inline static QVariantMap __fieldsToMap(QList<RPCField> iFields)
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

        QVariantMap lInnerMap   = lOutterMap[kInnerMapKey].toMap();
        for( auto lKey : lInnerMap.keys() ) {
            mFields << RPCField{lKey, lInnerMap[lKey]};
        }
    }else{
        qWarning() << "RPCMessage Json parse error, message balance" << lJsonParseError.offset << ":" << lUtf8Message.mid(lJsonParseError.offset);
    }
}

RPCMessage &RPCMessage::operator=(const RPCMessage &iOther)
{ _copy(iOther); return *this; }

RPCMessage &RPCMessage::operator<<(const RPCField &iField)
{ mFields << iField; return *this; }

QVariant RPCMessage::operator[](const QString iKey) const
{ return fieldValue(iKey); }

QVariant RPCMessage::fieldValue(const QString iKey) const
{
    for( auto lField : mFields ) {
        if( lField.Key == iKey )
            return lField.Value;
    }

    return QVariant();
}

QString RPCMessage::toMessage(const QString iUsername, const QByteArray iPublicKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    mUsername = iUsername;
    mSignatureEncoding = static_cast<int>(iKeyEncoding);

    QVariantMap lInnerMap       = __fieldsToMap(fields());
    QByteArray  lData           = QJsonDocument::fromVariant(lInnerMap).toJson(QJsonDocument::Compact);
    QVariantMap lOutterMap;


    if( iKeyEncoding == RPCMessage::KeyEncoding::None ) {
        mSignature = "";
    }else{
        enum Digest::HashTypes lHashType;
        switch( iKeyEncoding ) {

        case RPCMessage::KeyEncoding::SHA512:
        default:
            lHashType = Digest::HashTypes::SHA512;
            break;
        }

        mSignature = Digest::sign(iPublicKey,lData,lHashType);
    }

    lOutterMap[kUsernameKey]            = mUsername;
    lOutterMap[kSignatureKey]           = mSignature;
    lOutterMap[kSignatureEncodingKey]   = mSignatureEncoding;
    lOutterMap[kInnerMapKey]            = lInnerMap;

    return QString::fromUtf8( QJsonDocument::fromVariant(lOutterMap).toJson(QJsonDocument::Compact) );
}

QString RPCMessage::username() const
{ return mUsername; }

QByteArray RPCMessage::signature() const
{ return mSignature; }

RPCMessage::KeyEncoding RPCMessage::signatureKeyEncoding() const
{ return static_cast<RPCMessage::KeyEncoding>(mSignatureEncoding); }

void RPCMessage::setFields(QList<RPCField> iFields)
{ mFields = iFields; }

QList<RPCField> RPCMessage::fields() const
{ return mFields; }

QString RPCMessage::toMessage(QList<RPCField> iFields, const QString iUsername, const QByteArray iPublicKey, RPCMessage::KeyEncoding iKeyEncoding)
{
    RPCMessage  lMessage;
    lMessage.setFields(iFields);
    return lMessage.toMessage(iUsername, iPublicKey, iKeyEncoding);
}

void RPCMessage::_copy(const RPCMessage &iOther)
{
    if( &iOther != this ) {
        mUsername           = iOther.mUsername;
        mSignature          = iOther.mSignature;
        mSignatureEncoding  = iOther.mSignatureEncoding;
        mFields             = iOther.mFields;
    }
}
