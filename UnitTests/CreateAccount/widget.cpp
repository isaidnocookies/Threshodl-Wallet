#include "widget.h"
#include "ui_widget.h"

#include "rpcmessagecreateaccountrequest.h"

#include <QDebug>
#include <QFile>
#include <rpcmessagecreateaccountreply.h>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    mConnection = new RPCConnection{this};
    connect( mConnection, &RPCConnection::connected, this, &Widget::connectedToServer );
    connect( mConnection, &RPCConnection::disconnected, this, &Widget::disconnectedFromServer );
    connect( mConnection, &RPCConnection::socketError, this, &Widget::socketError );
    connect( mConnection, &RPCConnection::sslErrors, this, &Widget::sslErrors );
    connect( mConnection, &RPCConnection::failedToSendTextMessage, this, &Widget::failedToSendMessage );
    connect( mConnection, &RPCConnection::sentTextMessage, this, &Widget::sentMessage );
    connect( mConnection, &RPCConnection::textMessageReceived, this, &Widget::receivedMessage );

    QFile lFile{QStringLiteral("/tmp/ca.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        mSslConfiguration = QSslConfiguration::defaultConfiguration();
        mSslConfiguration.setCaCertificates(mSslConfiguration.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        lFile.close();
        mConnection->setSslConfiguration(mSslConfiguration);
    }else{
        qFatal("Failed to open CA cert.");
    }

    ui->statusLabel->setVisible(false);

    BitcoinInterface    lBI;

    auto lWallets = lBI.createWallets();
    if( lWallets.isEmpty() ) {
        qFatal("Failed to generate a bitcoin wallet!");
    }

    mBitcoinWallet = lWallets.takeFirst();

    // Generate a cert so we get a pair of keys to use, we don't need the cert
    Certificate lThisCert;
    lThisCert.setServerDefaults();
    lThisCert.selfSign();

    mThisPrivateKey = lThisCert.encryptionKey()->privateToPEM();
    mThisPublicKey = lThisCert.encryptionKey()->publicToPEM();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::connectedToServer()
{
    qDebug() << "Connected to server.";
    ui->statusLabel->setText("Connected.");

    mConnection->sendTextMessage(RPCMessageCreateAccountRequest::create(mThisPublicKey,ui->accountNameLineEdit->text(),mThisPrivateKey));
}

void Widget::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";
    ui->statusLabel->setText("Disconnected.");
    ui->goButton->setEnabled(true);
}

void Widget::failedToSendMessage()
{
    qDebug() << "Failed to sent message.";
}

void Widget::sentMessage()
{
    qDebug() << "Sent message.";
}

void Widget::receivedMessage()
{
    qDebug() << "Received message.";

    QString lMessage = mConnection->nextTextMessage();
    qDebug() << lMessage;

    RPCMessageCreateAccountReply    lReply{lMessage};
    qDebug() << "Reply code:" << static_cast<int>(lReply.replyCode());
}

void Widget::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    ui->statusLabel->setText(QStringLiteral("SocketError %1").arg(static_cast<int>(iError)));
    ui->goButton->setEnabled(true);
}

void Widget::sslErrors(const QList<QSslError> iErrors)
{
    int lIndex = 0;

    QString lErrorStr;

    qDebug() << "Ssl Errors:";
    for( auto lError : iErrors ) {
        if( lErrorStr.isEmpty() )
            lErrorStr = QStringLiteral("%1 %2").arg(lIndex).arg(lError.errorString());
        else
            lErrorStr = QStringLiteral("%1 | %2 %3").arg(lErrorStr).arg(lIndex).arg(lError.errorString());

        qDebug() << lIndex++ << lError.errorString();
    }

    ui->statusLabel->setText(lErrorStr);
    ui->goButton->setEnabled(true);
}

void Widget::on_goButton_pressed()
{
    QString lServerAddress  = ui->serverUrlLineEdit->text();
    QUrl    lUrl;

    if( lServerAddress.contains("://") )
        lUrl = QUrl::fromUserInput(lServerAddress);
    else
        lUrl = QUrl::fromUserInput(QStringLiteral("wss://%1").arg(lServerAddress));

    ui->statusLabel->setVisible(true);
    ui->statusLabel->setText(tr("Connecting..."));
    ui->goButton->setEnabled(false);

    mConnection->open(lUrl);
}
