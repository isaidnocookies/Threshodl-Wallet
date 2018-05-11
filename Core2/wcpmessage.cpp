#include "wcpmessage.h"
#include "digest.h"

#include <QDebug>
#include <QVariantMap>
#include <QJsonDocument>

#define     kWCPVersionKey          "ver"
#define     kUsernameKey            "user"
#define     kSignatureKey           "sig"
#define     kSignatureEncodingKey   "sige"
#define     kInnerMapKey            "data"

inline static QVariantMap __fieldsToMap(QList<WCPField> iFields)
{
    QVariantMap lMap;

    for( auto &lField : iFields ) {
        lMap[lField.Key] = lField.Value;
    }

    return lMap;
}

WCPMessage::WCPMessage(const WCPMessage &iOther)
{ _copy(iOther); }

WCPMessage::WCPMessage(const QString iMessage)
{
    QByteArray          lUtf8Message    = iMessage.toUtf8();
    QJsonParseError     lJsonParseError;

    QVariantMap lOutterMap = QJsonDocument::fromJson(lUtf8Message,&lJsonParseError).toVariant().toMap();
    if( lJsonParseError.error == QJsonParseError::NoError ) {
        mWCPVersion         = lOutterMap[kWCPVersionKey].toString();
        mUsername           = lOutterMap[kUsernameKey].toString();
        mSignature          = QByteArray::fromBase64(lOutterMap[kSignatureKey].toByteArray());
        mSignatureEncoding  = lOutterMap[kSignatureEncodingKey].toInt();
        mDataForSignature   = QByteArray::fromBase64(lOutterMap[kInnerMapKey].toByteArray());

        QVariantMap lInnerMap = QJsonDocument::fromJson(mDataForSignature,&lJsonParseError).toVariant().toMap();
        if( lJsonParseError.error == QJsonParseError::NoError ) {
            for( auto lKey : lInnerMap.keys() ) {
                mFields << WCPField{lKey, lInnerMap[lKey]};
            }
        }else{
            qWarning() << "WCPMessage Json parse error (inner), message balance" << lJsonParseError.offset << ":" << mDataForSignature.mid(lJsonParseError.offset);
        }
    }else{
        qWarning() << "WCPMessage Json parse error (outter), message balance" << lJsonParseError.offset << ":" << lUtf8Message.mid(lJsonParseError.offset);
    }
}

WCPMessage &WCPMessage::operator=(const WCPMessage &iOther)
{ _copy(iOther); return *this; }

WCPMessage &WCPMessage::operator<<(const WCPField &iField)
{ mFields << iField; return *this; }

QVariant WCPMessage::operator[](const QString iKey) const
{ return fieldValue(iKey); }

QVariant WCPMessage::fieldValue(const QString iKey) const
{
    for( auto lField : mFields ) {
        if( lField.Key == iKey )
            return lField.Value;
    }

    return QVariant();
}

QString WCPMessage::toMessage(const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    mUsername = iUsername;
    mSignatureEncoding = static_cast<int>(iKeyEncoding);

    QVariantMap lInnerMap       = __fieldsToMap(fields());
    mDataForSignature           = QJsonDocument::fromVariant(lInnerMap).toJson(QJsonDocument::Compact);
    QVariantMap lOutterMap;


    if( iKeyEncoding == WCPMessage::KeyEncoding::None ) {
        mSignature = "";
    }else{
        enum Digest::HashTypes lHashType;
        switch( iKeyEncoding ) {

        case WCPMessage::KeyEncoding::SHA512:
        default:
            lHashType = Digest::HashTypes::SHA512;
            break;
        }

        mSignature = Digest::sign(iPrivateKey,mDataForSignature,lHashType);
    }

    lOutterMap[kWCPVersionKey]          = mWCPVersion;
    lOutterMap[kUsernameKey]            = mUsername;
    lOutterMap[kSignatureKey]           = mSignature.toBase64();
    lOutterMap[kSignatureEncodingKey]   = mSignatureEncoding;
    lOutterMap[kInnerMapKey]            = mDataForSignature.toBase64();

    return QString::fromUtf8( QJsonDocument::fromVariant(lOutterMap).toJson(QJsonDocument::Compact) );
}

QString WCPMessage::wcpVersion() const
{ return mWCPVersion; }

QString WCPMessage::username() const
{ return mUsername; }

QByteArray WCPMessage::signature() const
{ return mSignature; }

WCPMessage::KeyEncoding WCPMessage::signatureKeyEncoding() const
{ return static_cast<WCPMessage::KeyEncoding>(mSignatureEncoding); }

QByteArray WCPMessage::dataForSignature() const
{ return mDataForSignature; }

void WCPMessage::setFields(QList<WCPField> iFields)
{ mFields = iFields; }

QList<WCPField> WCPMessage::fields() const
{ return mFields; }

QString WCPMessage::toMessage(QList<WCPField> iFields, const QString iUsername, const QByteArray iPrivateKey, WCPMessage::KeyEncoding iKeyEncoding)
{
    WCPMessage  lMessage;
    lMessage.setFields(iFields);
    return lMessage.toMessage(iUsername, iPrivateKey, iKeyEncoding);
}

void WCPMessage::_copy(const WCPMessage &iOther)
{
    if( &iOther != this ) {
        mWCPVersion         = iOther.mWCPVersion;
        mUsername           = iOther.mUsername;
        mSignature          = iOther.mSignature;
        mSignatureEncoding  = iOther.mSignatureEncoding;
        mDataForSignature   = iOther.mDataForSignature;
        mFields             = iOther.mFields;
    }
}
