#include "rpcmessages.h"
#include "digest.h"

#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>

#define     kStringElement_HasSignature     0
#define     kStringElement_Signature        1
#define     kStringElement_Data             2
#define     kStringElementCount             3

#define     kRPCVersion     "a"
#define     kRPCVersionKey  "v"
#define     kUsernameKey    "u"
#define     kPublicKeyKey   "p"
#define     kCommandKey     "c"
#define     kReplyCodeKey   "r"

#define     kCommand_CreateUsername     "createUsername"
#define     kCommand_LoginUsername      "loginUsername"
#define     kCommand_LogoutUsername     "logoutUsername"
#define     kCommand_DeleteUsername     "deleteUsername"

static QByteArray _sign(const QByteArray iData, const QByteArray iPublicKey, KeyEncoding iKeyEncoding)
{
    EncryptionKey *     lKey        = new EncryptionKey{iPublicKey,false};
    Digest::HashTypes   lHashType;

    switch( iKeyEncoding ) {
    case KeyEncoding::Sha512:
    default:
        lHashType = Digest::HashTypes::SHA512;
        break;
    }

    return Digest::sign(lKey,iData,lHashType);
}

static QString _createUnauthenticatedMessage(const QByteArray iData, const QByteArray iSign) {
    if( iData.isEmpty() || iSign.isEmpty() ) return QString();
    QByteArray lSign = iSign.toBase64();
    return QStringLiteral("1\n%1\n%2").arg(lSign.constData()).arg(iData.constData());
}

static QString _createAuthenticatedMessage(const QByteArray iData) {
    if( iData.isEmpty() ) return QString();
    return QStringLiteral("0\n0\n%1").arg(iData.constData());
}

static QVariantMap _createMapFromData(const QByteArray iData)
{
    QJsonParseError lJsonError;
    QVariantMap     lMap        = QJsonDocument::fromJson(iData, &lJsonError).toVariant().toMap();
    if( lJsonError.error == QJsonParseError::NoError )
        return lMap;
    return QVariantMap();
}

static QVariantMap _createMapFromData(const QString iData)
{
    return _createMapFromData(iData.toUtf8());
}

QString MessagePayloadKey::ProtocolVersion() const
{ return QStringLiteral(kRPCVersionKey); }

QString MessagePayloadKey::Username() const
{ return QStringLiteral(kUsernameKey); }

QString MessagePayloadKey::PublicKey() const
{ return QStringLiteral(kPublicKeyKey); }

QString OnClient::CreateUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding)
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kUsernameKey]      = iUsername;
    lJsonMap[kPublicKeyKey]     = iPublicKey;
    lJsonMap[kCommandKey]       = kCommand_CreateUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createUnauthenticatedMessage( lJson, _sign(lJson,iPublicKey,iKeyEncoding) );
}

QString OnClient::LoginUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding)
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kUsernameKey]      = iUsername;
    lJsonMap[kPublicKeyKey]     = iPublicKey;
    lJsonMap[kCommandKey]       = kCommand_LoginUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createUnauthenticatedMessage( lJson, _sign(lJson,iPublicKey,iKeyEncoding) );
}

QString OnClient::LogoutUsername()
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kCommandKey]       = kCommand_LogoutUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}

QString OnClient::DeleteUsername()
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kCommandKey]       = kCommand_DeleteUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}

QString OnClient::GeneralReply(qint32 iReplyCode)
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kReplyCodeKey]     = iReplyCode;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}

bool OnServer::MessageHasSignature(const QString iMessage)
{
    if( ! iMessage.isEmpty() || iMessage.at[0] == '1' ) return true;
    return false;
}

QByteArray OnServer::GetSignature(const QString iMessage)
{
    QStringList lElements = iMessage.split("\n");
    if( lElements.size() == kStringElementCount ) {
        return lElements[kStringElement_Signature].toUtf8();
    }
    return QByteArray();
}

QVariantMap OnServer::GetMessagePayload(const QString iMessage)
{
    QStringList lElements = iMessage.split("\n");
    if( lElements.size() == kStringElementCount ) {
        return _createMapFromData(lElements[kStringElement_Data]);
    }
    return QVariantMap();
}

QByteArray OnServer::GetCommandFromPayload(const QVariantMap iPayload)
{
    if( iPayload.contains(kCommandKey) ) {
        return iPayload[kCommandKey].toByteArray();
    }
    return QByteArray();
}
