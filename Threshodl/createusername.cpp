#include "createusername.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkRequest>

CreateUsername::CreateUsername(QObject *parent) : QObject(parent)
{
//    mNetworkManager = new QNetworkAccessManager();
//    connect(mNetworkManager, &QNetworkAccessManager::finished, this, &CreateUsername::requestComplete);
}

void CreateUsername::create(QString iUsername)
{
    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QString                 lReturnedUsername;

    QString lSeed, lPublic, lPrivate;
    bool lSuccess = false;

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    jsonData.insert("username", iUsername);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/userAccount/create/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lSeed = lMyMap["seed"].toString();
            lPublic = lMyMap["publickey"].toString();
            QString lPrivate = lMyMap["privatekey"].toString();
            lReturnedUsername = lMyMap["username"].toString();
            lSuccess = true;
        } else {
            lSuccess = false;
        }
    }

    if (!lSuccess) {
        lSuccess = false;
        lSeed = "";
        lPublic = "";
        lPrivate = "";
        qDebug() << "Error.... Cannot create account...";
    }

    emit usernameCreated(lSuccess, lReturnedUsername, lSeed, lPublic, lPrivate);
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

QString CreateUsername::changeUsername(QString iNewUsername)
{
    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QString                 lReturnedUsername;

    bool lSuccess = false;

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    jsonData.insert("username", iNewUsername);
    jsonData.insert("publickey", QString(mPublicKey));

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/userAccount/changeUsername/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lReturnedUsername = lMyMap["username"].toString();
            lSuccess = true;
        } else {
            lSuccess = false;
        }
    }

    if (!lSuccess) {
        lReturnedUsername = "";
    }

    mUsername = lReturnedUsername;

    return lReturnedUsername;
}

void CreateUsername::setUserData(QString iPrivateKey, QString iPublicKey, QString iUsername)
{
    mPublicKey = iPublicKey;
    mPrivateKey = iPrivateKey;
    mUsername = iUsername;
}

void CreateUsername::requestComplete(QNetworkReply *reply)
{
    QByteArray lReplyText = reply->readAll();
    qDebug() << lReplyText;

    auto lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

    if (lMyMap["success"].toBool()) {
        QString lSeed = lMyMap["seed"].toString();
        QString lPublic = lMyMap["publickey"].toString();
        QString lPrivate = lMyMap["privatekey"].toString();
        setUserData(mPrivateKey, mPublicKey, mUsername);
        emit usernameCreated(true, mUsername, lSeed, lPublic, lPrivate);
    } else {
        qDebug() << reply->errorString();
        emit usernameCreated(false, "", "", "", "");
    }
}
