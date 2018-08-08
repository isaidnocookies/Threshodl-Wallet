#include "createusername.h"

#include "globalsandconstants.h"
#include <QJsonDocument>
#include <QJsonObject>

CreateUsername::CreateUsername(QObject *parent) : QObject(parent)
{
    mNetworkManager = new QNetworkAccessManager(this);

    connect(mNetworkManager, &QNetworkAccessManager::finished, this, &CreateUsername::requestComplete);
}

void CreateUsername::create(QString iUsername)
{
    mUsername = iUsername;

    QJsonObject jsonData;
    jsonData.insert("username", iUsername);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QUrl lUrl = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/userAccount/mnemonicSeed"));

    QNetworkRequest lRequest;
    lRequest.setUrl(lUrl);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    mNetworkManager->post(lRequest, request_body);
}

void CreateUsername::requestComplete(QNetworkReply *reply)
{
    QByteArray lReplyText = reply->readAll();
    qDebug() << lReplyText;

    auto lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

    if (lMyMap["success"].toBool()) {
        QString lSeed = lMyMap["seed"].toString();
        emit usernameCreated(true, mUsername, lSeed, "public", "private");
    } else {
        emit usernameCreated(false, "", "", "", "");
    }
}
