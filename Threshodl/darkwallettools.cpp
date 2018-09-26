#include "darkwallettools.h"
#include "core.h"
#include "cryptowallet.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>

#include "SmtpClient/smtpclient.h"
#include "SmtpClient/mimeattachment.h"
#include "SmtpClient/mimehtml.h"

DarkWalletTools::DarkWalletTools(QObject *parent) : QObject(parent)
{

}

void DarkWalletTools::setUserDetails(QString iUsername, QString iPublicKey, QString iPrivateKey)
{
    mUsername = iUsername;
    mPublicKey = iPublicKey;
    mPrivateKey = iPrivateKey;
}

QString DarkWalletTools::estimateFeesForDark(QString iAmount, QString iShortname)
{
    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QString                 lFee = "-1";
    QString                 lBreaks = "-1";

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    jsonData.insert("coin", iShortname);
    jsonData.insert("amount", iAmount);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/dark/estimateBreaksAndFees/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lFee = lMyMap["feeEstimate"].toString();
        }
    } else {
        qDebug() << lReply->errorString();
    }
    return lFee;
}

bool DarkWalletTools::sendWallets(QVariantList iWallets, QString fromUser, QString iCryptoType, QString toEmailAddress, QString toUser, ErrorCodes::DarkErrors &oError)
{
    QString lAmount;
    QByteArray lAttachment;

    if (iWallets.size() > 0) {
        if (getAttachmentFile(iWallets, iCryptoType, lAmount, lAttachment)) {
            if (sendEmail(lAttachment, lAmount, toEmailAddress, toUser, fromUser, iCryptoType)) {
                if (transferDarkWallets(iWallets, fromUser, mPublicKey, mPrivateKey, toUser, mPublicKey)) {
                    oError = ErrorCodes::DarkErrors::None;
                    return true;
                } else {
                    oError = ErrorCodes::DarkErrors::TransferFailed;
                }
            } else {
                oError = ErrorCodes::DarkErrors::EmailFailed;
            }
        } else {
            oError = ErrorCodes::DarkErrors::AttachmentFailed;
        }
    } else {
        oError =  ErrorCodes::DarkErrors::InvalidAmount;
    }

    return false;
}

bool DarkWalletTools::getAttachmentFile(QVariantList iWallets, QString iCryptoShortname, QString &oAmount, QByteArray &oAttachment)
{
        QByteArray      lAttachment;
        QJsonObject     lJson;
        QJsonArray      lWalletArray;
        QStringMath     lTotalValueOfWallets;

        QList<CryptoWallet> lWalletsToSend;

        if (iWallets.size() > 0) {
            for (auto lW : iWallets) {
                auto lNewWallet = CryptoWallet(lW.toByteArray());
                lWalletsToSend.append(lNewWallet);
                lTotalValueOfWallets = lTotalValueOfWallets + lNewWallet.value();
                lWalletArray.push_back(QString(lNewWallet.toData()));
            }
        } else {
            oAmount = "0.00";
            oAttachment = QByteArray();
            return false;
        }

        lJson.insert("Action", "Dark Transaction");
        lJson.insert("Amount", lTotalValueOfWallets.toString());
        lJson.insert("TimeStamp", QString::number(QDateTime::currentMSecsSinceEpoch()));
        lJson.insert("Notes", QString("Dark %1 Transaction").arg(iCryptoShortname));
        lJson.insert("Type", iCryptoShortname);

        lJson.insert("Wallets", lWalletArray);

        QJsonDocument lJsonDoc (lJson);
        lAttachment = lJsonDoc.toJson();

        oAmount = lTotalValueOfWallets.toString();
        oAttachment = lAttachment;
        return true;
}

bool DarkWalletTools::sendEmail(QByteArray iAttachment, QString iAmount, QString toEmail, QString toUser, QString fromUser, QString iCryptoShortname)
{
    SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

    MimeMessage lMessage;
    EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
    EmailAddress lToEmail(toEmail, toEmail);

    MimeAttachment lAttachment(iAttachment, QString("%1.threshodl").arg(iCryptoShortname));

    MimeHtml lHtmlBody(QString("<html>"
                               "<h3>Threshodl Dark Transaction</h3>"
                               "<br>"
                               "<p>Hello %1! You have %2%3 from %4! Open up the attached file with your Threshodl to import your coins into your Dark Wallet."
                               "Once this package is imported, you will not be able to import it again. Thank you!</p>"
                               "<br><br>"
                               "Sent at %5 on %6"
                               "<br><br>"
                               "For more information, please visit www.threshodl.com or contact us at support@threshodl.com"
                               "<br><br>"
                               "</html>").arg(toUser).arg(iAmount).arg(iCryptoShortname).arg(fromUser).arg(QTime::currentTime().toString()).arg(QDate::currentDate().toString(AppInfo::myDateFormat())));

    lMessage.setSubject(QString("Threshodl - You have incoming %1!").arg(iCryptoShortname));
    lMessage.addRecipient(&lToEmail);
    lMessage.setSender(&lFromEmail);

    lMessage.addPart(&lHtmlBody);
    lMessage.addPart(&lAttachment);

    if(lClient.connectToHost()) {
        if (lClient.login("admin@threebx.com", "H0lyP33rP@1d", SmtpClient::AuthLogin)) {
            if (lClient.sendMail(lMessage)) {
                //success
                qDebug() << "Success";
                return true;
            } else {
                //failure
                qDebug() << "Failure";
            }
        } else {
            qDebug() << "AUTH failed for smtp client";
        }
    } else {
        qDebug() << "Failed to connect to smtp host";
    }
    return false;
}

bool DarkWalletTools::transferDarkWallets(QVariantList iWallets, QString iFromUser, QString iPublicKey, QString iPrivateKey, QString toUser, QString iAuthMessage)
{
    Q_UNUSED(iPrivateKey) Q_UNUSED(iAuthMessage) Q_UNUSED(iFromUser)

    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    bool                    lSuccess = false;

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonArray lWalletUidArray;

    for (auto lWallet : iWallets) {
        lWalletUidArray.append(QString(CryptoWallet(lWallet.toByteArray()).address()));
    }

    QJsonObject jsonData;
    jsonData.insert("ownerId", iPublicKey);
    jsonData.insert("newOwnerUsername", toUser);
    jsonData.insert("uid", lWalletUidArray);
    jsonData.insert("message", iPublicKey);
    jsonData.insert("password", THRESHODL_PASSWORD);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/dark/send/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lSuccess = lMyMap["confirmed"].toBool();
        } else {
            lSuccess = false;
        }
    }

    if (!lSuccess) {
        lSuccess = false;

    }
    return lSuccess;
}
