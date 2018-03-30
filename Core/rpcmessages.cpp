#include "rpcmessages.h"
#include "digest.h"

#include <QVariant>
#include <QVariantMap>
#include <QJsonDocument>

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
    return QStringLiteral("%1\n%2").arg(lSign.constData()).arg(iData.constData());
}

static QString _createAuthenticatedMessage(const QByteArray iData) {
    if( iData.isEmpty() ) return QString();
    return QStringLiteral("%1").arg(iData.constData());
}

QString CreateMessage::CreateUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding)
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

QString CreateMessage::LoginUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding)
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

QString CreateMessage::LogoutUsername()
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kCommandKey]       = kCommand_LogoutUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}

QString CreateMessage::DeleteUsername()
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kCommandKey]       = kCommand_DeleteUsername;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}

QString CreateMessage::GeneralReply(qint32 iReplyCode)
{
    QVariantMap lJsonMap;
    QByteArray  lJson;

    lJsonMap[kRPCVersionKey]    = kRPCVersion;
    lJsonMap[kReplyCodeKey]     = iReplyCode;

    lJson = QJsonDocument::fromVariant(lJsonMap).toJson(QJsonDocument::Compact);
    return _createAuthenticatedMessage(lJson);
}
