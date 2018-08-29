#include "createusername.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkRequest>

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
    QUrl lUrl = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/userAccount/create/"));

    QNetworkRequest lRequest;
    lRequest.setUrl(lUrl);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    mNetworkManager->post(lRequest, request_body);
}

void CreateUsername::recoverAccount(QString iSeed)
{
    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;

    QString lPublic, lPrivate, lUsername;
    bool lSuccess = false;

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    jsonData.insert("seed", iSeed);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/userAccount/recover/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lUsername = lMyMap["username"].toString();
            lPublic = lMyMap["publickey"].toString();
            lPrivate = lMyMap["privatekey"].toString();
            lSuccess = true;
        } else {
            lSuccess = false;
        }
    }

    if (!lSuccess) {
        lSuccess = false;
        lPublic = "";
        lPrivate = "";
        qDebug() << "Error.... Can not update bright wallet balance...";
    }

    emit usernameCreated(lSuccess, lUsername, iSeed, lPublic, lPrivate);
}

void CreateUsername::requestComplete(QNetworkReply *reply)
{
    QByteArray lReplyText = reply->readAll();
    qDebug() << lReplyText;

    auto lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

    if (lMyMap["success"].toBool()) {
        QString lSeed = lMyMap["seed"].toString();
        QString lPublic = lMyMap["publicKey"].toString();
        QString lPrivate = lMyMap["privateKey"].toString();
        emit usernameCreated(true, mUsername, lSeed, lPublic, lPrivate);
    } else {
        emit usernameCreated(false, "", "", "", "");
    }
}
