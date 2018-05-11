#ifndef WCPMESSAGE_H
#define WCPMESSAGE_H

#include "wcpfield.h"

#include <QList>

#define kWCPVersionV1           "1"
#define kWCPVersionV2           "2"
#define kWCPVersionCurrent      kWCPVersionV2
#define kFieldKey_Command       "command"

class WCPMessage
{
public:
    enum class KeyEncoding {
        None    = 0x0,
        SHA512  = 0x1
    };

    typedef enum KeyEncoding KeyEncoding;

    WCPMessage() = default;
    WCPMessage(const WCPMessage &iOther);
    WCPMessage(const QString iMessage);

    WCPMessage &operator=(const WCPMessage &iOther);
    WCPMessage &operator<<(const WCPField &iField);
    QVariant operator[](const QString iKey) const;
    QVariant fieldValue(const QString iKey) const;

    QString toMessage(const QString iUsername, const QByteArray iPrivateKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

    QString     wcpVersion() const;             // Immutable
    QString     username() const;
    QByteArray  signature() const;              // Only valid if constructed from a message or signed
    KeyEncoding signatureKeyEncoding() const;   // Only valid if constructed from a message or signed
    QByteArray  dataForSignature() const;       // Only valid if constructed from a message or signed

    void setFields(QList<WCPField> iFields);
    QList< WCPField > fields() const;

    static QString toMessage(QList< WCPField > iFields, const QString iUsername, const QByteArray iPrivateKey, KeyEncoding iKeyEncoding = KeyEncoding::SHA512);

protected:
    void _copy(const WCPMessage &iOther);

    QString             mWCPVersion         = QStringLiteral(kWCPVersionCurrent);
    QString             mUsername;                  // Only valid if constructed from a message
    QByteArray          mSignature;                 // Only valid if constructed from a message
    QByteArray          mDataForSignature;          // Only valid if constructed from a message
    int                 mSignatureEncoding  = 0x0;  // Only valid if constructed from a message or signed
    QList< WCPField >   mFields;
};

#endif // WCPMESSAGE_H
