#ifndef RPCMESSAGE_H
#define RPCMESSAGE_H

#include "rpcfield.h"

#include <QList>

class RPCMessage
{
public:
    enum class KeyEncoding {
        None    = 0x0,
        SHA512  = 0x1
    };

    typedef enum KeyEncoding KeyEncoding;

    RPCMessage() = default;
    RPCMessage(const RPCMessage &iOther);
    RPCMessage(const QString iMessage);

    RPCMessage &operator=(const RPCMessage &iOther);
    RPCMessage &operator<<(const RPCField &iField);

    QString toMessage(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

    QString     username() const;
    QByteArray  signature() const;               // Only valid if constructed from a message or signed
    KeyEncoding signatureKeyEncoding() const;   // Only valid if constructed from a message or signed

    QList< RPCField > fields() const;

    static QString toMessage(QList< RPCField > iFields, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

protected:
    void _copy(const RPCMessage &iOther);

    QString             mUsername;                  // Only valid if constructed from a message
    QByteArray          mSignature;                 // Only valid if constructed from a message
    int                 mSignatureEncoding = 0x0;   // Only valid if constructed from a message or signed
    QList< RPCField >   mFields;
};

#endif // RPCMESSAGE_H
