#include "sendtodarkview.h"
#include "ui_sendtodarkview.h"

#include "globalsandstyle.h"

SendToDarkView::SendToDarkView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendToDarkView)
{
    ui->setupUi(this);

    ui->convertButton->setStyleSheet(lightBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(lightBackgroundStyleSheet());

    ui->progressBar->setVisible(false);

    mConnection = new WCPConnection{this};

    mConfirmingOwnership = false;

    connect( mConnection, &WCPConnection::connected, this, &SendToDarkView::connectedToServer );
    connect( mConnection, &WCPConnection::disconnected, this, &SendToDarkView::disconnectedFromServer );
    connect( mConnection, &WCPConnection::socketError, this, &SendToDarkView::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &SendToDarkView::sslErrors );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &SendToDarkView::failedToSendMessage );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &SendToDarkView::sentMessage );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &SendToDarkView::receivedMessage );

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

void SendToDarkView::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
    mBalance = mActiveUser->getBrightBalance();
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mBalance.toString()));

    mUsername = mActiveUser->getUsername();
    mPrivateKey = mActiveUser->getPrivateKey();
}

void SendToDarkView::on_closeButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void SendToDarkView::on_convertButton_pressed()
{
    QStringMath lValue = ui->amountLineEdit->text();
    ui->warningLabel->setText("");

    if (lValue <= "0.0" || lValue > mActiveUser->getBrightBalance()) {
        ui->warningLabel->setText("Please input valid amount");
        return;
    }

    if (lValue >= "0.0" && !QStringMath::isMultipleOf(lValue.toString(), "0.0001")) {
        ui->warningLabel->setText("The value must be a multiple of 0.0001");
        return;
    }

    if (!mActiveUser->isBrightWalletSettled()) {
        ui->warningLabel->setWordWrap(true);
        ui->warningLabel->setText("Bright wallet is unconfirmed! Please wait and try again when it is settled.");
        return;
    }

    if (ui->confirmCheckBox->isChecked()) {
        // Allow transfer
        startProgressBarAndDisable();
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
    } else {
        // Don't allow
        ui->warningLabelForCheck->setText("*REQUIRED");
    }
}

void SendToDarkView::connectedToServer()
{
    qDebug() << "Connected to server.";
    mTransactionId = QString("%1.%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(QString::number(qrand() % 10000));
    int lAmountOfInputs;

    if (!mConfirmingOwnership) {
        if (mActiveUser->getNumberOfUnspentTransactions(mActiveUser->getBrightWallets(), ui->amountLineEdit->text(), lAmountOfInputs)) {
            if (lAmountOfInputs <= 0) {
                qDebug() << "Bright wallet has no unspent transactions";
                ui->warningLabel->setWordWrap(true);
                ui->warningLabel->setText("Your wallet does not have unspent transactions.");
            }
            mConnection->sendTextMessage(WCPMessageCreateMicroWalletPackageRequest::createBtc(ui->amountLineEdit->text(),lAmountOfInputs,1,currentChain(),mTransactionId,mUsername,mPrivateKey));
        } else {
            qDebug() << "Failed to get unspent transactions";
            ui->warningLabel->setWordWrap(true);
            ui->warningLabel->setText("Failed to get wallet information. Please try again.");
        }
    } else {
        // confirming ownership of wallets with server
        mConnection->sendTextMessage(WCPMessageClaimNewMicroWalletsRequest::create(mWalletsToConfirm, mTransactionId, mUsername, mPrivateKey));
    }

}

void SendToDarkView::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failures
    ui->warningLabel->setText("[1] Error, please try again!");
    stopProgressBarAndEnable();
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

    // got reply
    QString lMessage = mConnection->nextTextMessage();
    WCPMessageCreateMicroWalletPackageReply lReply {lMessage};

    QString lCommand = lReply.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

    if (lCommand == WCPMessageCreateMicroWalletPackageReply::commandValue()) {
        if (mTransactionId == lReply.transactionId()) {
            switch(lReply.replyCode()) {
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::Success:
                if (completeToDarkTransaction(lReply.microWalletsData(),lReply.estimatedFees())) {
                    mConfirmingOwnership = true;

                    if (mConnection->isConnected()) {
                        connectedToServer();
                    } else {
                        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
                        mConnection->open(lUrl);
                    }
                } else {
                    ui->warningLabel->setText("Conversion Failed!");
                    emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                }
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnableToGrindUpCryptoCurrency:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::Unauthorized:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::UnhandledCryptoType:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError1:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            default:
                break;
            }
        }
    } else if (lCommand == WCPMessageClaimNewMicroWalletsReply::commandValue()) {
        if (mTransactionId == lReply.transactionId()) {
            switch(lReply.replyCode()) {
            case WCPMessageClaimNewMicroWalletsReply::ReplyCode::Success:
                ui->warningLabel->setText("Conversion Complete!");
                ui->amountLineEdit->clear();
                break;
            case WCPMessageClaimNewMicroWalletsReply::ReplyCode::UnknownFailure:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            case WCPMessageCreateMicroWalletPackageReply::ReplyCode::InternalServerError1:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            default:
                ui->warningLabel->setText("Conversion Failed!");
                emit brightToDarkCompleted(false, QString(), QList<QByteArray>());
                break;
            }
        }
    }
    stopProgressBarAndEnable();
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

    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->warningLabel->setText("[4] Error, please try again!");
    }
    stopProgressBarAndEnable();
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

bool SendToDarkView::completeToDarkTransaction(QList<QByteArray> iData, QString iFeeAmount)
{
    QStringList             lTxids;
    QStringList             lValues;
    QStringList             lVouts;
    QList<QByteArray>       lPrivateKeys;
    QByteArray              lRawTransaction = QByteArray();
    QByteArray              lSignedHex = QByteArray();
    QList<BitcoinWallet>    lNewDarkWallets;
    QStringMath             lDarkWalletTotal;
    bool                    lReturn;

    QString lTotalBrightCoin = (mBalance - ui->amountLineEdit->text()).toString();

    mBalance = QStringMath(lTotalBrightCoin);
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mBalance.toString()));

    mActiveUser->setBrightBalance(lTotalBrightCoin);

    for (auto w : iData) {
        BitcoinWallet lW (w);
        lNewDarkWallets.append(lW);
        lDarkWalletTotal = lDarkWalletTotal + lW.value();
        mWalletsToConfirm.append(lW.walletId());
    }

    if (lNewDarkWallets.size() > 0 && lDarkWalletTotal > QStringMath("0.0")) {
        if (mActiveUser->fillDarkWallets(lNewDarkWallets, lDarkWalletTotal.toString(), iFeeAmount)) {
            for (auto w : lNewDarkWallets) {
                mActiveUser->addMicroWallet(w);
                qDebug() << "Adding.... " << BitcoinWallet(w).walletId();
            }
            emit updateBrightBalance(lTotalBrightCoin);
            emit brightToDarkCompleted(true, lTotalBrightCoin, iData);
            lReturn = true;
        } else {
            emit brightToDarkCompleted(false, mBalance.toString(), QList<QByteArray>());
            lReturn = false;
        }
    } else {
        emit brightToDarkCompleted(false, mBalance.toString(), QList<QByteArray>());
        lReturn = false;
    }

    stopProgressBarAndEnable();
    return lReturn;
}

void SendToDarkView::on_confirmCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1)

    ui->warningLabelForCheck->setText("");
}
