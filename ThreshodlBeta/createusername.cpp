#include "createusername.h"

#include "certificate.h"
#include "globalsandconstants.h"

CreateUsername::CreateUsername(QObject *parent) : QObject(parent)
{
    mConnection = new WCPConnection{this};
    connect( mConnection, &WCPConnection::connected, this, &CreateUsername::connectedToServer );
    connect( mConnection, &WCPConnection::disconnected, this, &CreateUsername::disconnectedFromServer );
    connect( mConnection, &WCPConnection::socketError, this, &CreateUsername::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &CreateUsername::sslErrors );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &CreateUsername::failedToSendMessage );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &CreateUsername::sentMessage );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &CreateUsername::receivedMessage );

    QFile lFile{QStringLiteral(":/ca.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        mSslConfiguration = QSslConfiguration::defaultConfiguration();
        mSslConfiguration.setCaCertificates(mSslConfiguration.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        lFile.close();
        mConnection->setSslConfiguration(mSslConfiguration);
    }else{
        qFatal("Failed to open CA cert.");
    }

    // Generate a cert so we get a pair of keys to use, we don't need the cert
    Certificate lThisCert;
    lThisCert.setServerDefaults();
    lThisCert.selfSign();

    mPrivateKey = lThisCert.encryptionKey()->privateToPEM();
    mPublicKey = lThisCert.encryptionKey()->publicToPEM();
}

void CreateUsername::create(QString iUsername)
{
    mUsername = iUsername;

    QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
    mConnection->open(lUrl);
}

void CreateUsername::connectedToServer()
{
    qDebug() << "Connected to server.";
    mConnection->sendTextMessage(WCPMessageCreateAccountRequest::create(mPublicKey,mUsername,mPrivateKey));
}

void CreateUsername::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";
    emit usernameCreated(false, QString(), QByteArray(), QByteArray());
}

void CreateUsername::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
    emit usernameCreated(false, QString(), QByteArray(), QByteArray());
}

void CreateUsername::sentMessage()
{
    qDebug() << "Sent message.";
}

void CreateUsername::receivedMessage()
{
    // got reply
    QString lMessage = mConnection->nextTextMessage();
    WCPMessageCreateAccountReply    lReply{lMessage};

    switch(lReply.replyCode()) {
        case WCPMessageCreateAccountReply::Success:
            qDebug() << "Success";
            // create account with new username
            emit usernameCreated(true, lReply.replyUsername(), mPublicKey, mPrivateKey);
            break;
        case WCPMessageCreateAccountReply::UsernameTaken:
            qDebug() << "Username Taken";
            emit usernameCreated(false, QString(), QByteArray(), QByteArray());
            break;
        default:
            qDebug() << "FUCK";
    }
}

void CreateUsername::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    emit usernameCreated(false, QString(), QByteArray(), QByteArray());
}

void CreateUsername::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;
    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
    }
    emit usernameCreated(false, QString(), QByteArray(), QByteArray());
}
