#include "sendtobrightview.h"
#include "ui_sendtobrightview.h"

#include "globalsandstyle.h"

#include <QFile>

SendToBrightView::SendToBrightView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendToBrightView)
{
    ui->setupUi(this);

    ui->convertButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(darkBackgroundStyleSheet());
    ui->progressBar->setVisible(false);

    mConnection = new WCPConnection{this};

    connect( mConnection, &WCPConnection::connected, this, &SendToBrightView::connectedToServer );
    connect( mConnection, &WCPConnection::disconnected, this, &SendToBrightView::disconnectedFromServer );
    connect( mConnection, &WCPConnection::socketError, this, &SendToBrightView::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &SendToBrightView::sslErrors );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &SendToBrightView::failedToSendMessage );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &SendToBrightView::sentMessage );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &SendToBrightView::receivedMessage );

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

SendToBrightView::~SendToBrightView()
{
    delete ui;
}

void SendToBrightView::setActiveUser(UserAccount *iUser)
{
    mActiveUser = iUser;
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1)").arg(mActiveUser->getDarkBalance().toString()));
}

void SendToBrightView::on_closeButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void SendToBrightView::on_convertButton_pressed()
{
    if (!mActiveUser->isDarkWalletsSettled()) {
        ui->warningLabel->setWordWrap(true);
        ui->warningLabel->setText("Please wait for dark wallet to be confirmed");
    }

    QStringMath lValue = ui->amountLineEdit->text();
    ui->warningLabel->clear();
    if (lValue <= "0.0" || lValue > mActiveUser->getDarkBalance()) {
        ui->warningLabel->setText("Please input valid amount");
        return;
    }

    if (lValue >= "0.0" && !QStringMath::isMultipleOf(lValue.toString(), "0.0001")) {
        ui->warningLabel->setText("The value must be a multiple of 0.0001");
        return;
    }

    if (!ui->confirmCheckBox->isChecked()) {
        ui->warningLabelForCheck->setText(QString("*Confirm to continue"));
    } else {
        startProgressBarAndDisable();
        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
        mConnection->open(lUrl);
    }
}

void SendToBrightView::connectedToServer()
{
    QStringList lWalletIds;
    bool lSuccess;
    QList<BitcoinWallet> lWalletsToComplete = getWalletsToComplete(ui->amountLineEdit->text(), lSuccess);

    if (!lSuccess) {

        // INCOMPLETE UNTIL BLOCKCHAIN STUFF IS SORTED OUT
        ui->warningLabel->setText("Failed because can't make change...");
        sendToBrightComplete(false);
        stopProgressBarAndEnable();
        return;
        // INCOMPLETE...

        mBreakdownMicroWallet = new BreakdownMicroWallet;
        mBreakdownMicroWallet->setActiveUser(mActiveUser);

        connect (mBreakdownMicroWallet, &BreakdownMicroWallet::microWalletBreakdownComplete, this, &SendToBrightView::microWalletBreakdownComplete);
        connect (mBreakdownMicroWallet, &BreakdownMicroWallet::completeMicroWallet, this, &SendToBrightView::completeMicroWallet);
        connect (mBreakdownMicroWallet, &BreakdownMicroWallet::newMicroWallets, this, &SendToBrightView::newMicroWallets);

        mBreakdownMicroWallet->getMicroWallets(mActiveUser->getDarkWallets().first());
    } else {
        if (lWalletsToComplete.count() == 0) {
            mConnection->close();
            ui->warningLabel->setText("ERROR! Conversion failed.");
            return;
        }

        for (auto w : lWalletsToComplete) {
            lWalletIds.append(w.walletId());
        }

        qDebug() << "Connected to server.";
        mTransactionId = QString("%1.%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(qrand() % 100);
        mConnection->sendTextMessage(WCPMessageCompleteMicroWalletsRequest::create(lWalletIds, mTransactionId, mActiveUser->getUsername(), mActiveUser->getPrivateKey()));
    }

}

void SendToBrightView::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failure
    ui->warningLabel->setText("[1] Error, please try again!");
    stopProgressBarAndEnable();
}

void SendToBrightView::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
    ui->warningLabel->setText("[2] Error, please try again!");

    //failed
    stopProgressBarAndEnable();
}

void SendToBrightView::sentMessage()
{
    qDebug() << "Sent message.";
}

void SendToBrightView::receivedMessage()
{
    qDebug() << "Received message.";

    // got reply
    QString lMessage = mConnection->nextTextMessage();
    WCPMessageCompleteMicroWalletsReply lReply {lMessage};
    bool lTreatAsFailure = false;

    if (lReply.transactionId() != mTransactionId) {
        //ignore
        qDebug() << "Message not for us";
        return;
    }

    QString lCommand = lReply.fieldValue(QStringLiteral(kFieldKey_Command)).toString();

    if (lCommand == WCPMessageCompleteMicroWalletsReply::commandValue()) {
        switch(lReply.replyCode()) {
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::Success:
            qDebug() << "Success";
            break;
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreUnauthorized:
            qDebug() << "OneOrMoreUnauthorized Error";
            lTreatAsFailure = true;
            break;
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::OneOrMoreMicroWalletsDoNotExist:
            qDebug() << "OneOrMoreMicroWalletsDoNotExist Error";
            lTreatAsFailure = true;
            break;
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError1:
            // Database error, cant talk to database
            qDebug() << "InternalServerError1";
            lTreatAsFailure = true;
            break;
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError2:
            // Some wallets were completed.. Result data--if size of data is the error code, its bad. if its not bad, its not bad (size of error is char)
            qDebug() << "InternalServerError2 Error";
            break;
        case WCPMessageCompleteMicroWalletsReply::ReplyCode::InternalServerError3:
            // Can't delete microwallet from internal database. Microwallets were complete tho... Treat as success.
            qDebug() << "InternalServerError3 Error";
            break;
        default:
            //WCPMessageCompleteMicroWalletsReply::ReplyCode::UnknownFailure
            qDebug() << "UnknownFailure Error";
            lTreatAsFailure = true;
            break;
        }
    }

    if (lTreatAsFailure) {
        //roll back changes...
        sendToBrightComplete(false);
    } else {
        completeWalletsAndAdd(lReply.walletPartialKeys());
        sendToBrightComplete(true);
    }

    stopProgressBarAndEnable();
}

void SendToBrightView::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    ui->warningLabel->setText("[3] Error, please try again!");
    stopProgressBarAndEnable();
}

void SendToBrightView::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;
    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->warningLabel->setText("[4] Error, please try again!");
    }

    stopProgressBarAndEnable();
}

void SendToBrightView::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->amountLineEdit->setEnabled(false);
    ui->convertButton->setEnabled(false);
    ui->confirmCheckBox->setEnabled(false);
    ui->closeButton->setEnabled(false);
}

void SendToBrightView::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->amountLineEdit->setEnabled(true);
    ui->convertButton->setEnabled(true);
    ui->confirmCheckBox->setEnabled(true);
    ui->closeButton->setEnabled(true);
}

void SendToBrightView::completeWalletsAndAdd(QMap<QString, QByteArray> iData)
{
    // parse out wallet parts
    QList<BitcoinWallet> lPartialWallets = mActiveUser->getPendingToSendDarkWallets();
    qDebug() << iData.keys() << " keys back from server to complete";
    QList<BitcoinWallet> lCompletedWallets;
    QStringMath lCompletedWalletTotalBalance;

    for (auto w: lPartialWallets) {
        BitcoinWallet lWallet = w;

        if (*(static_cast<char *>(iData[w.walletId()].data())) == WCPMessageCompleteMicroWalletsReply::InternalServerError2) {
            // key is an error, dont complete wallet
            lWallet.setOwner(mActiveUser->getUsername());
            lWallet.setIsMicroWallet(true);
            mActiveUser->addMicroWallet(lWallet);
        }

        lWallet.setPrivateKey(w.privateKey().append(iData[w.walletId()]));
        lWallet.setOwner(mActiveUser->getUsername());
        lWallet.setIsMicroWallet(false);
        lWallet.setWif(BitcoinWallet::generateWifFromPrivateKey(lWallet.privateKey(),currentChain()));
        lCompletedWallets.append(lWallet);
        lCompletedWalletTotalBalance = lCompletedWalletTotalBalance + lWallet.value();
    }

    if (mActiveUser->sendDarkWalletsToBrightWallet(lCompletedWallets)) {
        qDebug() << "Send dark to bright complete. Check blockchain shit";
    }

    mActiveUser->setBrightBalance(mActiveUser->getBrightBalance() + lCompletedWalletTotalBalance);

    mActiveUser->updateBalancesForViews(mActiveUser->getBrightBalance().toString(), mActiveUser->getDarkBalance().toString());
    mActiveUser->clearPendingToSendDarkWallets();
    emit updateDarkBalanceOnDarkWallet();
    ui->confirmationLabel->setText(QString("Conversion Complete!"));
    ui->confirmCheckBox->setChecked(false);
    ui->availableBalanceLabel->setText(QString("(Available Balance: %1").arg(mActiveUser->getDarkBalance().toString()));

    mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), QString("%1 Bitcoin was sent to Bright from Dark.").arg(ui->amountLineEdit->text()));
    ui->amountLineEdit->clear();
}

QList<BitcoinWallet> SendToBrightView::getWalletsToComplete(QStringMath iValue, bool &oSuccess)
{
    QList<BitcoinWallet> lAllWallets = mActiveUser->getDarkWallets();
    QList<BitcoinWallet> lWalletsToSend;
    QList<BitcoinWallet> lRemainingWallets;
    QStringMath lValue = QStringMath();

    for (auto w : lAllWallets) {
        if (lValue + w.value() <= iValue) {
            lWalletsToSend.append(w);
            lValue = lValue + w.value();
            qDebug() << "Added to complete list";
        } else {
            lRemainingWallets.append(w);
            qDebug() << "Added to remaining list";
        }
    }

    if (lValue < iValue) {
        //need to make change...
        oSuccess = false;
        return QList<BitcoinWallet>();
    }

    if (lValue != iValue) {
        qDebug() << "lValue is not zero... something happen";
        mActiveUser->clearPendingToSendDarkWallets();
        return QList<BitcoinWallet>();
    }

    qDebug() << lWalletsToSend.size() << " Wallets to complete";

    mActiveUser->setDarkWallets(lRemainingWallets);
    mActiveUser->savePendingToSendDarkWallets(lWalletsToSend);

    oSuccess = true;
    return lWalletsToSend;
}

void SendToBrightView::on_amountLineEdit_textChanged(const QString &arg1)
{
    if (arg1.size() > 0) {
        ui->confirmCheckBox->setChecked(false);
        ui->warningLabel->clear();
        ui->confirmationLabel->clear();
    }
}

void SendToBrightView::newMicroWallets(bool iSuccess)
{
    Q_UNUSED (iSuccess)
}

void SendToBrightView::completeMicroWallet(bool iSuccess)
{
    Q_UNUSED (iSuccess)
}

void SendToBrightView::microWalletBreakdownComplete(bool iSuccess)
{
    if (iSuccess) {
        qDebug() << "SUCCESS for breaking down microwallet. Continue with trying to send coin...";

        if (mConnection->isConnected()) {
            connectedToServer();
        } else {
            on_convertButton_pressed();
        }
    } else {
        //error....
        qDebug() << "FAILURE to break down microwallets before send. Trying again...";
        ui->warningLabel->setText("Failed to convert back to bright. Please try again.");
    }
}

void SendToBrightView::sendToBrightComplete(bool iSuccess)
{
    if (iSuccess) {
        qDebug() << "Successfully sent dark to bright!";
        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Dark wallets were sent to bright!");
    } else {
        for (auto w : mActiveUser->getPendingToSendDarkWallets()) {
            mActiveUser->addMicroWallet(w);
        }
        mActiveUser->clearPendingToSendDarkWallets();
        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Failed to send dark to bright..");
        qDebug() << "Failed to send dark to bright. Rolling back changes...";
    }
}
