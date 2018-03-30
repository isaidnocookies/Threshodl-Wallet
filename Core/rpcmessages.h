#ifndef RPCMESSAGES_H
#define RPCMESSAGES_H

#include <QByteArray>
#include <QString>

enum class KeyEncoding {
    Sha512  = 0x1
};

typedef enum KeyEncoding KeyEncoding;

namespace CreateMessage {
    // Functions that are done when NOT logged in:
    QString CreateUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::Sha512);
    QString LoginUsername(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::Sha512);

    // Functions only done AFTER logged in:
    QString LogoutUsername();
    QString DeleteUsername();

    // Reply messages
    QString GeneralReply(qint32 iReplyCode = 0);
}

#endif // RPCMESSAGES_H
