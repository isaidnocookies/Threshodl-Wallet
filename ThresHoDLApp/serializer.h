#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QByteArray>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariant>
#include <QVariantList>

#include "globalsandstyle.h"

class Serializer
{
public:
    static QByteArray serializeWallet (QByteArray iPublicKey, QByteArray iPrivKey, QString iAmount);
    static void unSerializeWallet (const QByteArray iInput, QByteArray &iPublic, QByteArray &iPrivKey, QString &iAmount);

    static QByteArray serializeDarkQrCode(QString iThresAddress, QString iEmailAddress);
    static void unserializeDarkQrCode(const QByteArray iData, QString &iThresAddress, QString &iEmailAddress);

//    static QByteArray serializeDarkMessage();
//    static void unSerializeDarkMessage();
};

#endif // SERIALIZER_H
