#ifndef RPCMESSAGES_H
#define RPCMESSAGES_H

#include <QByteArray>
#include <QString>
#include <QVariantMap>

enum class KeyEncoding {
    Sha512  = 0x1
};

typedef enum KeyEncoding KeyEncoding;

namespace MessagePayloadKey {
    QString ProtocolVersion() const;
    QString Username() const;
    QString PublicKey() const;
}

namespace OnClient {
    // Functions that are done when NOT logged in:
    QString CreateUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::Sha512);
    QString LoginUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::Sha512);

    // Functions only done AFTER logged in:
    QString LogoutUsername();
    QString DeleteUsername();

    // Reply messages
    QString GeneralReply(qint32 iReplyCode = 0);
}

namespace OnServer {
    bool MessageHasSignature(const QString iMessage);
    QByteArray GetSignature(const QString iMessage);
    QVariantMap GetMessagePayload(const QString iMessage);
    QByteArray GetCommandFromPayload(const QVariantMap iPayload);
}

#endif // RPCMESSAGES_H
