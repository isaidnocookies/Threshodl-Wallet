#include "serializer.h"

QByteArray Serializer::serializeWallet(QByteArray iPublicKey, QByteArray iPrivKey, QString iAmount)
{
    QJsonObject lJsonObject;
    lJsonObject.insert(jsonPublicKey(), QJsonValue::fromVariant(iPublicKey));
    lJsonObject.insert(jsonPrivateKey(), QJsonValue::fromVariant(iPrivKey));
    lJsonObject.insert(jsonAmount(), iAmount);

    QJsonDocument lDoc (lJsonObject);
    return lDoc.toJson();
}

void Serializer::unSerializeWallet(const QByteArray iInput, QByteArray &iPublic, QByteArray &iPrivKey, QString &iAmount)
{
    QJsonDocument lJsonDoc = QJsonDocument::fromJson(iInput);
    QJsonObject lJsonObject = lJsonDoc.object();

    iPublic = lJsonObject[jsonPublicKey()].toVariant().toByteArray();
    iPrivKey = lJsonObject[jsonPrivateKey()].toVariant().toByteArray();
    iAmount = lJsonObject[jsonAmount()].toString();
}

QByteArray Serializer::serializeDarkQrCode(QString iThresAddress, QString iEmailAddress)
{
    QJsonObject lJsonObject;
    lJsonObject.insert(jsonEmailAddress(), iEmailAddress);
    lJsonObject.insert(jsonThreshodlAddress(), iThresAddress);

    QJsonDocument lDoc (lJsonObject);
    return lDoc.toJson();
}

void Serializer::unserializeDarkQrCode(const QByteArray iData, QString &iThresAddress, QString &iEmailAddress)
{
    QJsonDocument lJsonDoc = QJsonDocument::fromJson(iData);
    QJsonObject lJsonObject = lJsonDoc.object();

    iThresAddress = lJsonObject[jsonThreshodlAddress()].toString();
    iEmailAddress = lJsonObject[jsonEmailAddress()].toString();
}

