#include "sendtodarkview.h"
#include "ui_sendtodarkview.h"

#include "globalsandstyle.h"
#include "certificate.h"
#include "encryptionkey.h"

#include "rpcmessagecreatemicrowalletpackagerequest.h"

SendToDarkView::SendToDarkView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendToDarkView)
{
    ui->setupUi(this);

    ui->convertButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(lightBackgroundStyleSheet());

    ui->progressBar->setVisible(false);

    mConnection = new RPCConnection{this};

    connect( mConnection, &RPCConnection::connected, this, &SendToDarkView::connectedToServer );
    connect( mConnection, &RPCConnection::disconnected, this, &SendToDarkView::disconnectedFromServer );
    connect( mConnection, &RPCConnection::socketError, this, &SendToDarkView::socketError );
    connect( mConnection, &RPCConnection::sslErrors, this, &SendToDarkView::sslErrors );
    connect( mConnection, &RPCConnection::failedToSendTextMessage, this, &SendToDarkView::failedToSendMessage );
    connect( mConnection, &RPCConnection::sentTextMessage, this, &SendToDarkView::sentMessage );
    connect( mConnection, &RPCConnection::textMessageReceived, this, &SendToDarkView::receivedMessage );

    QFile lFile{QStringLiteral(":/ca.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        mSslConfiguration = QSslConfiguration::defaultConfiguration();
        mSslConfiguration.setCaCertificates(mSslConfiguration.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        lFile.close();
        mConnection->setSslConfiguration(mSslConfiguration);
    }else{
        qFatal("Failed to open CA cert.");
    }
}

SendToDarkView::~SendToDarkView()
{
    delete ui;
}

void SendToDarkView::setBalance(double iBalance)
{
    mBalance = iBalance;
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mBalance));
}

void SendToDarkView::setValues(QByteArray iPriv, QString iUsername)
{
    mUsername = iUsername;
    mPrivateKey = iPriv;
}

void SendToDarkView::on_closeButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void SendToDarkView::on_convertButton_pressed()
{
    if (ui->confirmCheckBox->isChecked()) {
        // Allow transfer
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
        startProgressBarAndDisable();
    } else {
        // Don't allow
        ui->warningLabelForCheck->setText("*REQUIRED");
    }
}

void SendToDarkView::connectedToServer()
{
    qDebug() << "Connected to server.";
    mConnection->sendTextMessage(RPCMessageCreateMicroWalletPackageRequest::create("BTC", ui->amountLineEdit->text(), mUsername, mPrivateKey));
}

void SendToDarkView::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failure
    stopProgressBarAndEnable();
    ui->warningLabel->setText("[1] Error, please try again!");
}

void SendToDarkView::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
    ui->warningLabel->setText("[2] Error, please try again!");

    //failed
    stopProgressBarAndEnable();
}

void SendToDarkView::sentMessage()
{
    qDebug() << "Sent message.";
}

void SendToDarkView::receivedMessage()
{
    qDebug() << "Received message.";

    stopProgressBarAndEnable();

    // got reply
    QString lMessage = mConnection->nextTextMessage();

    ui->warningLabel->setText(lMessage);
//    RPCMessageCreateAccountReply    lReply{lMessage};

//    switch(lReply.replyCode()) {
//        default:
//            qDebug() << "FUCK";
//    }
}

void SendToDarkView::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    ui->warningLabel->setText("[3] Error, please try again!");
    stopProgressBarAndEnable();
}

void SendToDarkView::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;
    stopProgressBarAndEnable();

    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->warningLabel->setText("[4] Error, please try again!");
    }
}

void SendToDarkView::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->convertButton->setEnabled(false);
    ui->amountLineEdit->setEnabled(false);
    ui->closeButton->setEnabled(false);
    ui->confirmCheckBox->setEnabled(false);
}

void SendToDarkView::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->convertButton->setEnabled(true);
    ui->amountLineEdit->setEnabled(true);
    ui->closeButton->setEnabled(true);
    ui->confirmCheckBox->setEnabled(true);
}

void SendToDarkView::parseBitcoinPackage(QByteArray iData)
{
    double lTotalDarkCoin = 0;
    double lTotalBrightCoin = 0;

    ui->warningLabel->setText(QString(iData));
    // DO MORE STUFFS

//    lTotalBrightCoin = ui->amountLineEdit->text().toDouble();

    emit updateBrightBalance(lTotalBrightCoin);
    emit brightToDarkCompleted(lTotalBrightCoin, QList<BitcoinWallet>());
}

void SendToDarkView::on_confirmCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)

    ui->warningLabelForCheck->setText("");
}
