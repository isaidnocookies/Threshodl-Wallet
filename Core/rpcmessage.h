#ifndef RPCMESSAGE_H
#define RPCMESSAGE_H

#include "rpcfield.h"

#include <QList>

#define kFieldKey_Command   "command"

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
    QVariant operator[](const QString iKey) const;
    QVariant fieldValue(const QString iKey) const;

    QString toMessage(const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

    QString     username() const;
    QByteArray  signature() const;              // Only valid if constructed from a message or signed
    KeyEncoding signatureKeyEncoding() const;   // Only valid if constructed from a message or signed
    QByteArray  dataForSignature() const;       // Only valid if constructed from a message or signed

    void setFields(QList<RPCField> iFields);
    QList< RPCField > fields() const;

    static QString toMessage(QList< RPCField > iFields, const QString iUsername, const QByteArray iPublicKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

protected:
    void _copy(const RPCMessage &iOther);

    QString             mUsername;                  // Only valid if constructed from a message
    QByteArray          mSignature;                 // Only valid if constructed from a message
    QByteArray          mDataForSignature;          // Only valid if constructed from a message
    int                 mSignatureEncoding = 0x0;   // Only valid if constructed from a message or signed
    QList< RPCField >   mFields;
};

#endif // RPCMESSAGE_H
