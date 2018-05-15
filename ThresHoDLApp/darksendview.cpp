#include "darksendview.h"
#include "ui_darksendview.h"
#include "globalsandstyle.h"
#include "serializer.h"

#include "SmtpClient/smtpclient.h"
#include "SmtpClient/emailaddress.h"
#include "SmtpClient/mimeattachment.h"
#include "SmtpClient/mimemessage.h"
#include "SmtpClient/mimehtml.h"
#include "SmtpClient/mimemultipart.h"

#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

DarkSendView::DarkSendView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DarkSendView)
{
    ui->setupUi(this);

    ui->sendTransactionButton->setStyleSheet(darkBackgroundStyleSheet());
    ui->addressLineEdit->setStyleSheet(darkBackgroundStyleSheet());
    ui->amountLineEdit->setStyleSheet(darkBackgroundStyleSheet());
    ui->emailAddressLineEdit->setStyleSheet(darkBackgroundStyleSheet());

    ui->sendConfirmationLabel->setText("");
    ui->progressBar->setVisible(false);

    mConnection = new WCPConnection{this};

    connect( mConnection, &WCPConnection::connected, this, &DarkSendView::connectedToServer );
    connect( mConnection, &WCPConnection::disconnected, this, &DarkSendView::disconnectedFromServer );
    connect( mConnection, &WCPConnection::socketError, this, &DarkSendView::socketError );
    connect( mConnection, &WCPConnection::sslErrors, this, &DarkSendView::sslErrors );
    connect( mConnection, &WCPConnection::failedToSendTextMessage, this, &DarkSendView::failedToSendMessage );
    connect( mConnection, &WCPConnection::sentTextMessage, this, &DarkSendView::sentMessage );
    connect( mConnection, &WCPConnection::textMessageReceived, this, &DarkSendView::receivedMessage );

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

void DarkSendView::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
    ui->availableBalanceLabel->setText(QString("Available Balance: %1").arg(mActiveUser->getDarkBalance().toString()));
}

DarkSendView::~DarkSendView()
{
    delete ui;
}

void DarkSendView::on_closePushButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void DarkSendView::on_scanQrButton_pressed()
{
    ui->sendConfirmationLabel->setText("");

    mQrReaderView = new QrReader;

    connect (mQrReaderView, &QrReader::sendCameraCode, this, &DarkSendView::getQrCode);

    mQrReaderView->show();
    mQrReaderView->showMaximized();
}

void DarkSendView::on_sendTransactionButton_pressed()
{
    QStringMath lAmount     = QStringMath(ui->amountLineEdit->text());
    QString lAddress        = ui->addressLineEdit->text();
    QString lEmailAddress   = ui->emailAddressLineEdit->text();

    ui->sendWarningLabel->setText("");

    if (lAmount <= "0.0" || lAmount > mActiveUser->getDarkBalance()) {
        ui->sendWarningLabel->setText("Please input a correct amount");
        return;
    }

    if (!mActiveUser->isDarkWalletsSettled()) {
        ui->sendWarningLabel->setWordWrap(true);
        ui->sendWarningLabel->setText("Please wait for dark wallet to be confirmed");
    }

    if (lAmount == "0.0" || lAddress.isEmpty() || lEmailAddress.isEmpty() || !ui->confirmCheckBox->isChecked()) {
        ui->sendWarningLabel->setText("Please complete all fields and confirm!");
    } else if (false){//lAmount > mActiveUser->getDarkBalance()) {
        ui->sendWarningLabel->setText("You do not have enough Bitcoin");
    } else {
        QByteArray lTestMessage;
        startProgressBarAndDisable();

        bool lAttachmentSuccess = getAttachmentPackage(lTestMessage);

        if (!lAttachmentSuccess) {

            // INCOMPLETE UNTIL BLOCKCHAIN STUFF IS SORTED OUT
            ui->sendConfirmationLabel->setText("Failed because can't make change...");
            stopProgressBarAndEnable();
            return;
            // INCOMPLETE...

            mBreakdownMicroWallet = new BreakdownMicroWallet;
            mBreakdownAttempts = 3;
            mBreakdownMicroWallet->setActiveUser(mActiveUser);

            connect (mBreakdownMicroWallet, &BreakdownMicroWallet::microWalletBreakdownComplete, this, &DarkSendView::microWalletBreakdownComplete);
            connect (mBreakdownMicroWallet, &BreakdownMicroWallet::completeMicroWallet, this, &DarkSendView::completeMicroWallet);
            connect (mBreakdownMicroWallet, &BreakdownMicroWallet::newMicroWallets, this, &DarkSendView::newMicroWallets);

            mBreakdownMicroWallet->getMicroWallets(mActiveUser->getDarkWallets().first());
        } else {
            SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

            MimeMessage lMessage;
            EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
            EmailAddress lToEmail(lEmailAddress, lEmailAddress);

            MimeAttachment lAttachment(lTestMessage, "bitcoin.threshodl");

            MimeHtml lHtmlBody(QString("<html>"
                                       "<h3>Threshodl Dark Transaction</h3>"
                                       "<br>"
                                       "<p>Hello %1! You have %2 Bitcoin! Open up the attached file with your Threshodl to import your coins into your Dark Wallet."
                                       "Once this package is imported, you will not be able to import it again. Thank you!</p>"
                                       "<br><br>"
                                       "Sent at %3 on %4"
                                       "<br><br>"
                                       "</html>").arg(lAddress).arg(ui->amountLineEdit->text()).arg(QTime::currentTime().toString()).arg(QDate::currentDate().toString(myDateFormat())));

            lMessage.setSubject("Threshodl - You have Bitcoin!");
            lMessage.addRecipient(&lToEmail);
            lMessage.setSender(&lFromEmail);

            lMessage.addPart(&lHtmlBody);
            lMessage.addPart(&lAttachment);

            if(lClient.connectToHost()) {
                if (lClient.login("admin@threebx.com", "H0lyP33rP@1d", SmtpClient::AuthLogin)) {
                    if (lClient.sendMail(lMessage)) {
                        //success
                        qDebug() << "Success";

                        mTransactionID = QString("%1.%2").arg(QDateTime::currentMSecsSinceEpoch()).arg(QString::number(qrand() % 1000));
                        QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
                        mConnection->open(lUrl);
                    } else {
                        //failure
                        qDebug() << "Failure";
                        sentConfirmation(false);
                        stopProgressBarAndEnable();
                    }
                } else {
                    qDebug() << "AUTH failed for smtp client";
                    stopProgressBarAndEnable();
                }
            } else {
                qDebug() << "Failed to connect to smtp host";
                stopProgressBarAndEnable();
            }
        }
    }
}

void DarkSendView::getQrCode(QString iData)
{
    //unserialize stuffs
    QString lAddress;
    QString lEmail;

    Serializer::unserializeDarkQrCode(iData.toUtf8(), lAddress, lEmail);

    if ( !lAddress.isEmpty() && !lEmail.isEmpty()) {
        ui->addressLineEdit->setText(lAddress);
        ui->emailAddressLineEdit->setText(lEmail);
    }
}

void DarkSendView::newMicroWallets(bool iSuccess)
{
    //hmmm...
}

void DarkSendView::completeMicroWallet(bool iSuccess)
{
    //hmmm...
}

void DarkSendView::microWalletBreakdownComplete(bool iSuccess)
{
    if (iSuccess) {
        qDebug() << "SUCCESS for breaking down microwallet. Continue with trying to send coin...";
        on_sendTransactionButton_pressed();
    } else {
        //error....
        qDebug() << "FAILURE to break down microwallets before send. Trying again...";
        on_sendTransactionButton_pressed();
        mBreakdownAttempts--;
    }
}

void DarkSendView::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->sendTransactionButton->setEnabled(true);
}

void DarkSendView::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->sendTransactionButton->setEnabled(false);
}

bool DarkSendView::getAttachmentPackage(QByteArray &oData)
{
    QByteArray      lAttachment;
    QJsonObject     lJson;
    QJsonArray      lWalletArray;

    QList<BitcoinWallet> lWalletsToSend;
    QList<BitcoinWallet> lRemainingWallets;
    QStringMath lValue = QStringMath(ui->amountLineEdit->text());
    QStringMath lCounterValue = QStringMath();

    for (auto w : mActiveUser->getDarkWallets()) {
        if ( lCounterValue + w.value() <= lValue ) {
            lCounterValue = lCounterValue + w.value();
            lWalletsToSend.append(w);
            lWalletArray.push_back(QString(w.toData()));
        } else {
            lRemainingWallets.append(w);
        }

        if (lValue < lCounterValue) {
            qDebug() << "FUCK...";
            return false;
        }
    }

    if (lCounterValue < lValue) {
        qDebug() << "Can't make change...";
        return false;
    }

    mWalletsToSend_Pending = lWalletsToSend;
    mPendingAmountToSend = ui->amountLineEdit->text();

    mActiveUser->setDarkWallets(lRemainingWallets);
    mActiveUser->savePendingToSendDarkWallets(lWalletsToSend);

    lJson.insert("Action", "Import");
    lJson.insert("Amount", ui->amountLineEdit->text());
    lJson.insert("TransactionId", mTransactionID);
    lJson.insert("Notes", "Bitcoin dark transaction.");

    lJson.insert("Wallets", lWalletArray);

    QJsonDocument lJsonDoc (lJson);
    lAttachment = lJsonDoc.toJson();

    oData = lAttachment;
    return true;
}

void DarkSendView::on_amountLineEdit_textChanged(const QString &arg1)
{
    if (arg1.size() > 0) {
        ui->sendConfirmationLabel->setText("");
    }
}

void DarkSendView::on_addressLineEdit_textChanged(const QString &arg1)
{
    if (arg1.size() > 0) {
        ui->sendConfirmationLabel->setText("");
    }
}

void DarkSendView::on_emailAddressLineEdit_textChanged(const QString &arg1)
{
    if (arg1.size() > 0) {
        ui->sendConfirmationLabel->setText("");
    }
}

void DarkSendView::connectedToServer()
{
    qDebug() << "Connected to server.";
    QStringList lWalletList;

    for (auto w : mWalletsToSend_Pending) {
        lWalletList.append(w.walletId());
    }

    mConnection->sendTextMessage(WCPMessageReassignMicroWalletsRequest::create(ui->addressLineEdit->text(), lWalletList, mTransactionID, mActiveUser->getUsername(), mActiveUser->getPrivateKey()));
}

void DarkSendView::disconnectedFromServer()
{
    qDebug() << "Disconnected from server.";

    //catastrophic failure
    stopProgressBarAndEnable();
    ui->sendWarningLabel->setText("[1] Error, please try again!");
}

void DarkSendView::failedToSendMessage()
{
    qDebug() << "Failed to send message.";
    ui->sendWarningLabel->setText("[2] Error, please try again!");

    //failed
    stopProgressBarAndEnable();
}

void DarkSendView::sentMessage()
{
    qDebug() << "Sent message.";
}

void DarkSendView::receivedMessage()
{
    qDebug() << "Received message.";

    // got reply
    QString lMessage = mConnection->nextTextMessage();
    WCPMessageReassignMicroWalletsReply lReply{lMessage};

    if (mTransactionID != lReply.transactionId()) {
        //message isnt ours
        return;
    }

    switch(lReply.replyCode()) {
    case WCPMessageReassignMicroWalletsReply::ReplyCode::Success:
        qDebug() << "FUCK YEAH!!!";
        sentConfirmation(true);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsDoNotExist:
        // Server doesnt know where a wallet came from
        qDebug() << "FUCK 1";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::OneOrMoreWalletsUnauthorized:
        // I dont have ownership of one or more wallets - whole transfer failed
        qDebug() << "FUCK 2";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::SourceDoesNotExist:
        qDebug() << "FUCK 3";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::DestinationDoesNotExist:
        // Destination doesnt exist
        qDebug() << "FUCK 4";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::InternalServerError1:
        // Transfer never happen. internal problem
        qDebug() << "FUCK 5";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::InternalServerError2:
        // Wallets had to be rolled back (ownership) so proceed with caution - reintroduce wallets into dark - FAIL IT TODO:
        qDebug() << "FUCK 6";
        sentConfirmation(false);
        break;
    case WCPMessageReassignMicroWalletsReply::ReplyCode::InternalServerError3:
        // Lock all wallets. Wait for manual help
        qDebug() << "FUCK 7";
        sentConfirmation(false);
        break;
    default:
            qDebug() << "FUCK 8";
            sentConfirmation(false);
        break;
    }

    stopProgressBarAndEnable();
}

void DarkSendView::socketError(QAbstractSocket::SocketError iError)
{
    qDebug() << "SocketError:" << iError;
    ui->sendWarningLabel->setText("[3] Error, please try again!");
    stopProgressBarAndEnable();
}

void DarkSendView::sslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Ssl Errors:";

    int lIndex = 0;
    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->sendWarningLabel->setText("[4] Error, please try again!");
    }

    stopProgressBarAndEnable();
}

void DarkSendView::sentConfirmation(bool iSuccess)
{
    if (iSuccess) {
        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), QString("%1 Bitcoin sent via Dark Wallet!").arg(ui->amountLineEdit->text()));
        ui->amountLineEdit->setText("");
        ui->addressLineEdit->setText("");
        ui->emailAddressLineEdit->setText("");

        ui->sendConfirmationLabel->setText("Transfer Sent - Success!");

        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Transfer Successful! Dark Bitcoin has been sent.");
        ui->availableBalanceLabel->setText(QString("Available Balance: %1").arg((mActiveUser->getDarkBalance() - mPendingAmountToSend).toString()));
    } else {
        ui->sendConfirmationLabel->setText("Transfer Failed!");
        QList<BitcoinWallet> lWallets = mActiveUser->getPendingToSendDarkWallets();
        for (auto w : lWallets) {
            mActiveUser->addMicroWallet(w);
        }

        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Transfer FAILED! Dark Bitcoin has not been sent.");
    }
    mActiveUser->clearPendingToSendDarkWallets();

    ui->availableBalanceLabel->setText(QString("Available Balance: %1").arg(mActiveUser->getDarkBalance().toString()));
    mActiveUser->updateBalancesForViews(mActiveUser->getBrightBalance().toString(), mActiveUser->getDarkBalance().toString());
    emit updateBalance();

    stopProgressBarAndEnable();
}
