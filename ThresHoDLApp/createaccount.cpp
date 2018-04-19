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

    ui->createAccountButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->usernameLineEdit->setStyleSheet(lightBackgroundStyleSheet());

    ui->progressBar->setVisible(false);

    mConnection = new RPCConnection{this};
    connect( mConnection, &RPCConnection::connected, this, &CreateAccount::connectedToServer );
    connect( mConnection, &RPCConnection::disconnected, this, &CreateAccount::disconnectedFromServer );
    connect( mConnection, &RPCConnection::socketError, this, &CreateAccount::socketError );
    connect( mConnection, &RPCConnection::sslErrors, this, &CreateAccount::sslErrors );
    connect( mConnection, &RPCConnection::failedToSendTextMessage, this, &CreateAccount::failedToSendMessage );
    connect( mConnection, &RPCConnection::sentTextMessage, this, &CreateAccount::sentMessage );
    connect( mConnection, &RPCConnection::textMessageReceived, this, &CreateAccount::receivedMessage );

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

CreateAccount::~CreateAccount()
{
    delete ui;
}

void CreateAccount::on_createAccountButton_pressed()
{
//    emit createUserAccount(ui->usernameLineEdit->text(), "", "");
//    return;

    ui->warningLabel->setText("");
    if (!ui->usernameLineEdit->text().isEmpty()) {
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
        startProgressBarAndDisable();
    } else {
        ui->warningLabel->setStyleSheet("color: red;");
        ui->warningLabel->setText("*Please enter username");
    }
}

void CreateAccount::connectedToServer()
{
    qDebug() << "Connected to server.";
    mConnection->sendTextMessage(RPCMessageCreateAccountRequest::create(mPublicKey,ui->usernameLineEdit->text(),mPrivateKey));
}

void CreateAccount::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failure
    stopProgressBarAndEnable();
    ui->warningLabel->setText("[1] Error, please try again!");
}

void CreateAccount::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
    ui->warningLabel->setText("[2] Error, please try again!");

    //failed
    stopProgressBarAndEnable();
}

void CreateAccount::sentMessage()
{
    qDebug() << "Sent message.";

    //processing at server -- show loading bar
}

void CreateAccount::receivedMessage()
{
    qDebug() << "Received message.";

    stopProgressBarAndEnable();

    // got reply
    QString lMessage = mConnection->nextTextMessage();
    RPCMessageCreateAccountReply    lReply{lMessage};

    switch(lReply.replyCode()) {
        case RPCMessageCreateAccountReply::Success:
            qDebug() << "Success";
            // create account with new username
            emit createUserAccount(lReply.replyUsername(), mPrivateKey, mPublicKey);
            break;
        case RPCMessageCreateAccountReply::UsernameTaken:
            qDebug() << "Username Taken";
            ui->warningLabel->setText("Username has been taken!");
            break;
        default:
            qDebug() << "FUCK";
    }
}

void CreateAccount::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    ui->warningLabel->setText("[3] Error, please try again!");
    stopProgressBarAndEnable();
}

void CreateAccount::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;
    stopProgressBarAndEnable();

    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->warningLabel->setText("[4] Error, please try again!");
    }
}

void CreateAccount::createUser(QString iUsername)
{
    emit createUserAccount(iUsername, mPrivateKey, mPublicKey);
}

void CreateAccount::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->createAccountButton->setEnabled(false);
    ui->usernameLineEdit->setEnabled(false);
}

void CreateAccount::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->createAccountButton->setEnabled(true);
    ui->usernameLineEdit->setEnabled(true);
}
