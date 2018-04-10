#include "darksendview.h"
#include "ui_darksendview.h"
#include "globalsandstyle.h"

#include "SmtpClient/smtpclient.h"
#include "SmtpClient/emailaddress.h"
#include "SmtpClient/mimeattachment.h"
#include "SmtpClient/mimemessage.h"
#include "SmtpClient/mimehtml.h"

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
}

void DarkSendView::on_sendTransactionButton_pressed()
{
    QString lAmount         = ui->amountLineEdit->text();
    QString lAddress        = ui->addressLineEdit->text();
    QString lEmailAddress   = ui->emailAddressLineEdit->text();

    ui->sendWarningLabel->setText("");

    if (lAmount.isEmpty() || lAddress.isEmpty() || lEmailAddress.isEmpty() || !ui->confirmCheckBox->isChecked()) {
        ui->sendWarningLabel->setText("Please complete all fields and confirm!");
    } else {
        SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

        if(lClient.connectToHost()) {
            if (lClient.login("admin@threebx.com", "H0lyP33rP@1d", SmtpClient::AuthLogin)) {
                MimeMessage lMessage;
                EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
                EmailAddress lToEmail(lEmailAddress, lEmailAddress);

                QByteArray lTestMessage = "Testing file";
                MimeAttachment lAttachment(lTestMessage, "bitcoin.threshodl");
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

                if (lClient.sendMail(lMessage)) {
                    //success
                    qDebug() << "Success";
                } else {
                    //failure
                    qDebug() << "Failure";
                }
            }

//            return;
//            QUrl lUrl = QUrl::fromUserInput(QStringLiteral("wss://10.10.0.78:4431"));
//            mConnection->open(lUrl);
//            startProgressBarAndDisable();
        }
    }
}

void DarkSendView::sendConfirmation(bool iSuccess)
{
    if (iSuccess) {
        ui->sendConfirmationLabel->setText("Send Complete!");
    } else {
        ui->sendConfirmationLabel->setText("Send Canceled!");
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

void DarkSendView::on_amountLineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    ui->sendConfirmationLabel->setText("");
}

void DarkSendView::on_addressLineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    ui->sendConfirmationLabel->setText("");
}

void DarkSendView::on_emailAddressLineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    ui->sendConfirmationLabel->setText("");
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
    } else {
        ui->sendConfirmationLabel->setText("Transfer Failed!");
    }
}
