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
}

void DarkSendView::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
    ui->availableBalanceLabel->setText(QString("Available Balance: %1").arg(mActiveUser->getDarkBalance()));
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
    QString lAmount         = ui->amountLineEdit->text();
    QString lAddress        = ui->addressLineEdit->text();
    QString lEmailAddress   = ui->emailAddressLineEdit->text();

    ui->sendWarningLabel->setText("");


    // REVERSE SMTP CLIENT AND TRANSFERRING BITCOIN! Move smtp to message received!
    // This way we can do a check on if the threshodl address is valid before sending package


    if (lAmount.isEmpty() || lAddress.isEmpty() || lEmailAddress.isEmpty() || !ui->confirmCheckBox->isChecked()) {
        ui->sendWarningLabel->setText("Please complete all fields and confirm!");
    } else if (lAmount.toDouble() > mActiveUser->getDarkBalance()) {
        ui->sendWarningLabel->setText("You do not have enough Bitcoin");
    } else {
        SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

        MimeMessage lMessage;
        EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
        EmailAddress lToEmail(lEmailAddress, lEmailAddress);

        QByteArray lTestMessage = getAttachmentPackage();
        MimeAttachment lAttachment(lTestMessage, "bitcoin.threshodl");
        lAttachment.setContentType("application/json");
        lAttachment.addHeaderLine("Content-Disposition: attachment; filename=\"bitcoin.threshodl\"");
        lAttachment.setContentId("<bitcoin.threshodl>");
        lAttachment.setEncoding(MimePart::Encoding::Base64);

        MimeHtml lHtmlBody("<html>"
                           "<h3>Threshodl Dark Transaction</h3>"
                           "<br>"
                           "<p>You have Bitcoin! Open up the attached file with your Threshodl to import your coins into your Dark Wallet.</p>"
                           "<br><br>"
                           "</html>");
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
                    sendConfirmation(true);
                } else {
                    //failure
                    qDebug() << "Failure";
                    sendConfirmation(false);
                }
            }

            return;
            QUrl lUrl = QUrl::fromUserInput(QStringLiteral(TEST_SERVER_IP_ADDRESS));
            mConnection->open(lUrl);
            startProgressBarAndDisable();
        }
    }
}

void DarkSendView::sendConfirmation(bool iSuccess)
{
    if (iSuccess) {
        ui->sendConfirmationLabel->setText("Send Complete!");
        ui->amountLineEdit->setText("");
        ui->addressLineEdit->setText("");
        ui->emailAddressLineEdit->setText("");
        ui->confirmCheckBox->setChecked(false);
    } else {
        ui->sendConfirmationLabel->setText("Send Canceled!");
        ui->confirmCheckBox->setChecked(false);
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

QByteArray DarkSendView::getAttachmentPackage()
{
    QByteArray      lAttachment;
    QJsonObject     lJson;
    QJsonArray      lWalletArray;

    QList<BitcoinWallet> lWalletsToSend;
    QList<BitcoinWallet> lRemainingWallets;
    double lValue = ui->amountLineEdit->text().toDouble();

    for (auto w : mActiveUser->getDarkWallets()) {
        if (w.value().toDouble() <= lValue) {
            lValue -= w.value().toDouble();
            lWalletsToSend.append(w);
            lWalletArray.push_back(QString(w.toData()));
        } else {
            lRemainingWallets.append(w);
        }
    }

    mWalletsToSend_Pending = lWalletsToSend;

    mActiveUser->setDarkWallets(lRemainingWallets);
    mActiveUser->savePendingToSendDarkWallets(lWalletsToSend);

    lJson.insert("Amount", ui->amountLineEdit->text());
    lJson.insert("TransactionId", "00000000000000000");
    lJson.insert("Notes", "Bitcoin dark transaction.");

    lJson.insert("Wallets", lWalletArray);

    QJsonDocument lJsonDoc (lJson);
    lAttachment = lJsonDoc.toJson();

    return lAttachment;
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
//    mConnection->sendTextMessage();
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

    stopProgressBarAndEnable();

    // got reply
//    QString lMessage = mConnection->nextTextMessage();
//    RPCMessageCreateAccountReply    lReply{lMessage};

//    switch(lReply.replyCode()) {
//        case [something or other]:
//            break;
//        default:
//            qDebug() << "FUCK";
//    }
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
    stopProgressBarAndEnable();

    for( auto lError : iErrors ) {
        qDebug() << lIndex++ << lError.errorString();
        ui->sendWarningLabel->setText("[4] Error, please try again!");
    }
}

void DarkSendView::sentConfirmation(bool iSuccess)
{
    if (iSuccess) {
        ui->amountLineEdit->setText("");
        ui->addressLineEdit->setText("");
        ui->emailAddressLineEdit->setText("");

        ui->sendConfirmationLabel->setText("Transfer Sent - Success!");

        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Transfer Successful! Dark Bitcoin has been sent.");
    } else {
        ui->sendConfirmationLabel->setText("Transfer Failed!");
        QList<BitcoinWallet> lWallets = mActiveUser->getPendingToSendDarkWallets();
        for (auto w : lWallets) {
            mActiveUser->addMicroWallet(w);
        }

        mActiveUser->addNotification(QDate::currentDate().toString(myDateFormat()), "Transfer FAILED! Dark Bitcoin has not been sent.");
    }
    mActiveUser->clearPendingToSendDarkWallets();

    ui->availableBalanceLabel->setText(QString("Available Balance: %1").arg(mActiveUser->getDarkBalance()));
    mActiveUser->updateBalancesForMainWindow(mActiveUser->getBrightBalance(), mActiveUser->getDarkBalance());
    emit updateBalance();
}
