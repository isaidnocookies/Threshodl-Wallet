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

QString DarkWalletTools::estimateFeesForWithdrawal(int iNumWallets, QString iShortname)
{
    QNetworkAccessManager   *lNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QString                 lFee = "-1";
    QString                 lShortname = iShortname;

    if (lShortname.contains("d")) {
        iShortname = lShortname.remove("d");
    }

    connect(lNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    jsonData.insert("coin", lShortname);
    jsonData.insert("inputs", iNumWallets);
    jsonData.insert("outputs", 1);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();

    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/txFee/"));
    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = lNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        if (lMyMap["success"].toBool()) {
            lFee = lMyMap["fee"].toString();
        }
    } else {
        qDebug() << lReply->errorString();
    }
    return lFee;
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
        if (getAttachmentFile(iWallets, toUser, iCryptoType, lAmount, lAttachment)) {
            if (sendEmail(lAttachment, lAmount, toEmailAddress, toUser, fromUser, iCryptoType)) {
                if (transferDarkWallets(iWallets, fromUser, mPublicKey, mPrivateKey, toUser, mPublicKey)) {
                    oError = ErrorCodes::DarkErrors::None;
                    return true;
                } else {
                    oError = ErrorCodes::DarkErrors::TransferFailed;
                    if (sendFailureEmail(toEmailAddress, toUser, fromUser, iCryptoType)) {
                        qDebug() << "Failed to send failure email";
                    }
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

bool DarkWalletTools::completeWallets(QVariantList lWallets, QVariantList &oCompleteWallets)
{
    QVariantList lCompleteWallets;

    return false;
}

bool DarkWalletTools::getAttachmentFile(QVariantList iWallets, QString toUser, QString iCryptoShortname, QString &oAmount, QByteArray &oAttachment)
{
        QByteArray      lAttachment;
        QJsonObject     lJson;
        QJsonArray      lWalletArray;
        QStringMath     lTotalValueOfWallets;

        QList<CryptoWallet> lWalletsToSend;

        if (iWallets.size() > 0) {
            for (auto lW : iWallets) {
                auto lNewWallet = CryptoWallet(lW.toByteArray());
                lNewWallet.setOwner(toUser);
                qDebug() << lNewWallet.toData();
                lWalletsToSend.append(lNewWallet);
                lTotalValueOfWallets = lTotalValueOfWallets + lNewWallet.value();
                lWalletArray.push_back(QString(lNewWallet.toData()));
            }
        } else {
            oAmount = "0.00";
            oAttachment = QByteArray();
            return false;
        }

        lJson.insert("action", "Dark Transaction");
        lJson.insert("amount", lTotalValueOfWallets.toString());
        lJson.insert("timeStamp", QString::number(QDateTime::currentMSecsSinceEpoch()));
        lJson.insert("notes", QString("Dark %1 Transaction").arg(iCryptoShortname));
        lJson.insert("type", iCryptoShortname);

        lJson.insert("wallets", lWalletArray);

        QJsonDocument lJsonDoc (lJson);
        lAttachment = lJsonDoc.toJson();

        oAmount = lTotalValueOfWallets.toString();
        oAttachment = lAttachment;
        return true;
}

bool DarkWalletTools::sendEmail(QByteArray iAttachment, QString iAmount, QString toEmail, QString toUser, QString fromUser, QString iCryptoShortname)
{
    Q_UNUSED(fromUser)

    SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

    MimeMessage lMessage;
    EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
    EmailAddress lToEmail(toEmail, toEmail);

    MimeAttachment lAttachment(iAttachment, QString("%1.threshodl").arg(iCryptoShortname));

    MimeHtml lHtmlBody(QString("<html>"
                               "<h3>Threshodl Dark Transaction</h3>"
                               "<br>"
                               "<p>Hello %1! You have %2%3! Open up the attached file with your Threshodl to import your coins into your Dark Wallet."
                               "Once this package is imported, you will not be able to import it again. Thank you!</p>"
                               "<br><br>"
                               "Sent at %4 on %5"
                               "<br><br>"
                               "For more information, please visit www.threshodl.com or contact us at support@threshodl.com"
                               "<br><br>"
                               "</html>").arg(toUser).arg(iAmount).arg(iCryptoShortname).arg(QTime::currentTime().toString()).arg(QDate::currentDate().toString(AppInfo::myDateFormat())));

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

bool DarkWalletTools::sendFailureEmail(QString toEmail, QString toUser, QString fromUser, QString iCryptoShortname)
{
    Q_UNUSED(fromUser)

    SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

    MimeMessage lMessage;
    EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
    EmailAddress lToEmail(toEmail, toEmail);

    MimeHtml lHtmlBody(QString("<html>"
                               "<h3>Threshodl Dark Transaction Failed</h3>"
                               "<br>"
                               "<p>Hello %1! There was a problem with the transfer of the previous %2 wallets. Please disregard the previous email "
                               "and contact the sender to check on the status of the transaction. Apologies for any issues that this delay may have caused.</p>"
                               "<br><br>"
                               "Sent at %3 on %4"
                               "<br><br>"
                               "For more information, please visit www.threshodl.com or contact us at support@threshodl.com"
                               "<br><br>"
                               "</html>").arg(toUser).arg(iCryptoShortname).arg(QTime::currentTime().toString()).arg(QDate::currentDate().toString(AppInfo::myDateFormat())));

    lMessage.setSubject(QString("Threshodl - Error with previous %1 transaction!").arg(iCryptoShortname));
    lMessage.addRecipient(&lToEmail);
    lMessage.setSender(&lFromEmail);

    lMessage.addPart(&lHtmlBody);

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
        lWalletUidArray.append(QString(CryptoWallet(lWallet.toByteArray()).uid()));
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
