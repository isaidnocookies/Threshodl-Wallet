#ifndef CREATEUSERNAME_H
#define CREATEUSERNAME_H

#include "core.h"

#include <QObject>
#include <QDebug>
#include <QNetworkAccessManager>

class CreateUsername : public QObject
{
    Q_OBJECT
public:
    explicit CreateUsername(QObject *parent = nullptr);
    void create(QString iUsername);
    void recoverAccount(QString iSeed);
    QString changeUsername(QString iNewUsername);

signals:
    void usernameCreated(bool oSuccess, QString oUsername, QString oSeed, QString oPublicKey, QString oPrivateKey);

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
