#include "createaccount.h"
#include "ui_createaccount.h"

#include "globalsandstyle.h"
#include "certificate.h"
#include "encryptionkey.h"

#include "rpcmessagecreateaccountrequest.h"
#include "rpcmessagecreateaccountreply.h"

#include <QDebug>
#include <QFile>

CreateAccount::CreateAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CreateAccount)
{
    ui->setupUi(this);

    ui->createAccountButton->setStyleSheet("border-radius: 5px; background-color: #15AEFF; padding: 10px;");

    mConnection = new RPCConnection{this};
    connect( mConnection, &RPCConnection::connected, this, &CreateAccount::connectedToServer );
    connect( mConnection, &RPCConnection::disconnected, this, &CreateAccount::disconnectedFromServer );
    connect( mConnection, &RPCConnection::socketError, this, &CreateAccount::socketError );
    connect( mConnection, &RPCConnection::sslErrors, this, &CreateAccount::sslErrors );
    connect( mConnection, &RPCConnection::failedToSendTextMessage, this, &CreateAccount::failedToSendMessage );
    connect( mConnection, &RPCConnection::sentTextMessage, this, &CreateAccount::sentMessage );
    connect( mConnection, &RPCConnection::textMessageReceived, this, &CreateAccount::receivedMessage );

    mSslConfiguration = QSslConfiguration::defaultConfiguration();
    mSslConfiguration.setCaCertificates(mSslConfiguration.caCertificates() << QSslCertificate{serverCert(),QSsl::Pem});
    mConnection->setSslConfiguration(mSslConfiguration);

    // Generate a cert so we get a pair of keys to use, we don't need the cert
    Certificate lThisCert;
    lThisCert.setServerDefaults();
    lThisCert.selfSign();

    mPrivateKey = lThisCert.encryptionKey()->privateToPEM();
    mPublicKey = lThisCert.encryptionKey()->publicToPEM();
}

CreateAccount::~CreateAccount()
{
    delete ui;
}

void CreateAccount::on_createAccountButton_pressed()
{
    if (!ui->usernameLineEdit->text().isEmpty()) {
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral("wss://10.10.0.77:4431"));
        mConnection->open(lUrl);
//        createUser(ui->usernameLineEdit->text());
    } else {
        ui->warningLabel->setStyleSheet("color: red;");
        ui->warningLabel->setText("*Please enter username");
    }
}

void CreateAccount::connectedToServer()
{
    qDebug() << "Connected to server.";
    mConnection->sendTextMessage(RPCMessageCreateAccountRequest::create(mPublicKey,ui->usernameLineEdit->text(),mPrivateKey));

    //show loading bar?
}

void CreateAccount::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failure
}

void CreateAccount::failedToSendMessage()
{
    qDebug() << "Failed to send message.";

    //failed
}

void CreateAccount::sentMessage()
{
    qDebug() << "Sent message.";

    //processing at server -- show loading bar
}

void CreateAccount::receivedMessage()
{
    qDebug() << "Received message.";

    // got reply
    QString lMessage = mConnection->nextTextMessage();
    RPCMessageCreateAccountReply    lReply{lMessage};

    switch(lReply.replyCode()) {
        case RPCMessageCreateAccountReply::Success:
            qDebug() << "Success";
            break;
        case RPCMessageCreateAccountReply::UsernameTaken:
            qDebug() << "Username Taken";
            break;
        default:
            qDebug() << "FUCK";
    }
}

void CreateAccount::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
}

void CreateAccount::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;

    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
    }
}

void CreateAccount::createUser(QString iUsername)
{
    emit createUserAccount(iUsername, mPrivateKey, mPublicKey);
}
