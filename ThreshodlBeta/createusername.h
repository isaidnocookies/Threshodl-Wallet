#ifndef CREATEUSERNAME_H
#define CREATEUSERNAME_H

#include "core.h"

#include <QObject>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QSslKey>
#include <QDebug>
#include <QNetworkReply>

class CreateUsername : public QObject
{
    Q_OBJECT
public:
    explicit CreateUsername(QObject *parent = nullptr);
    void create(QString iUsername);

signals:
    void usernameCreated(bool oSuccess, QString oUsername, QString oSeed, QByteArray oPublicKey, QByteArray oPrivateKey);

private slots:
    void requestComplete(QNetworkReply *reply);

private:
    QNetworkAccessManager *mNetworkManager;

    QByteArray mPrivateKey;
    QByteArray mPublicKey;
    QString mUsername;

    QString mTransactionId;
};

#endif // CREATEUSERNAME_H
