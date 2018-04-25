#include "useraccount.h"
#include "globalsandstyle.h"
#include "bitcoinwallet.h"

#include <QDebug>
#include <QCoreApplication>

extern "C" {
#include <btc/chainparams.h>
#include <btc/ecc.h>
#include <btc/tool.h>
#include <btc/utils.h>

#include <btc/tx.h>
#include <btc/cstr.h>
#include <btc/ecc_key.h>
#include <btc/script.h>
#include <btc/utils.h>

#include <btc/tool.h>
#include <btc/bip32.h>

#include <btc/base58.h>
}

#include "SmtpClient/smtpclient.h"
#include "SmtpClient/emailaddress.h"
#include "SmtpClient/mimeattachment.h"
#include "SmtpClient/mimemessage.h"
#include "SmtpClient/mimehtml.h"
#include "SmtpClient/mimemultipart.h"

UserAccount::UserAccount(QObject *parent)
{
    Q_UNUSED(parent)

    QCoreApplication::setOrganizationDomain(theDomain());
    QCoreApplication::setApplicationName(theApplicationName());
    QCoreApplication::setOrganizationName(theOrganization());

    mAccountSettings = new QSettings;
    mBitcoinBlockchainInterface = new BitcoinBlockchainInterface;
    mBitcoinBlockchainInterface->setActiveUser(this);

    loadFromSettings();
}

void UserAccount::createNewAccount(QString iUsername, QByteArray iPriv, QByteArray iPub)
{
    mAccountSettings->setValue(usernameSetting(), iUsername);
    mAccountSettings->setValue(publicKeySetting(), iPub);
    mAccountSettings->setValue(privateKeySetting(), iPriv);

    mUsername = iUsername;
    mPrivateKey = iPriv;
    mPublicKey = iPub;
}

QString UserAccount::getUsername()
{
    return mUsername;
}

QByteArray UserAccount::getPublicKey()
{
    return mPublicKey;
}

QByteArray UserAccount::getPrivateKey()
{
    return mPrivateKey;
}

QString UserAccount::getEmail()
{
    return mEmail;
}

QList<QString> UserAccount::getNotifications()
{
    return mNotifications;
}

QList<BitcoinWallet> UserAccount::getBrightWallets()
{
    return mBrightWallet;
}

QList<BitcoinWallet> UserAccount::getDarkWallets()
{
    return mDarkWallet;
}

BitcoinWallet UserAccount::getBrightWallet(int iIndex)
{
    if (iIndex < mBrightWallet.size()) {
        return mBrightWallet.at(iIndex);
    } else if (mBrightWallet.size() > 0) {
        return mBrightWallet.at(0);
    } else {
        return BitcoinWallet::createNewBitcoinWallet(currentChain());
    }
}

void UserAccount::setUsername(QString iUsername)
{
    mUsername = iUsername;

    mAccountSettings->setValue(usernameSetting(), iUsername);
    mAccountSettings->sync();
}

void UserAccount::setKeys(QByteArray iPub, QByteArray iPriv)
{
    mPublicKey = iPub;
    mPrivateKey = iPriv;

    mAccountSettings->setValue(privateKeySetting(), iPriv);
    mAccountSettings->setValue(publicKeySetting(), iPub);
    mAccountSettings->sync();
}

void UserAccount::addNotification(QString iTime, QString iNotificationText)
{
    QList<QVariant> lNotifications;

    mNotifications.append(iTime);
    mNotifications.append(iNotificationText);

    if (mAccountSettings->contains(notificationsSetting())) {
        lNotifications = mAccountSettings->value(notificationsSetting()).toList();
    }

    lNotifications.append(QVariant(iTime));
    lNotifications.append(QVariant(iNotificationText));

    mAccountSettings->setValue(notificationsSetting(), QVariantList(lNotifications));
    mAccountSettings->sync();
}

void UserAccount::setNotifications(QList<QVariant> iNotifications)
{
    mNotifications.clear();
    mAccountSettings->remove(notificationsSetting());
    int lCount = iNotifications.size();
    QList<QVariant> lNotifications;

    for (int i = 0; i < lCount; i+=2) {

        mNotifications.append(iNotifications[i].toString());
        mNotifications.append(iNotifications[i+1].toString());

        lNotifications.append(QVariant(iNotifications[i].toString()));
        lNotifications.append(QVariant(iNotifications[i+1].toString()));

        mAccountSettings->setValue(notificationsSetting(), QVariantList(lNotifications));
        mAccountSettings->sync();
    }
}

void UserAccount::setAddresses(QString iEmail, QString iThreshodlAddress)
{
    mEmail = iEmail;

    mAccountSettings->setValue(addressSetting(), iThreshodlAddress);
    mAccountSettings->setValue(emailSetting(), iEmail);

    mAccountSettings->sync();
}

void UserAccount::savePendingToSendDarkWallets(QList<BitcoinWallet> iWallets)
{
    QList<QVariant> lPendingWallets;
    mAccountSettings->remove(darkWalletPendingSetting());

    for (auto w : iWallets) {
        lPendingWallets.append(w.toData());
    }

    mAccountSettings->setValue(darkWalletPendingSetting(), lPendingWallets);
    mAccountSettings->sync();
}

void UserAccount::clearPendingToSendDarkWallets()
{
    mAccountSettings->remove(darkWalletPendingSetting());
    mAccountSettings->sync();
}

QList<BitcoinWallet> UserAccount::getPendingToSendDarkWallets()
{
    QList<BitcoinWallet> lPendingWallets;
    QList<QVariant> lWallets = mAccountSettings->value(darkWalletPendingSetting()).toList();

    for (auto w : lWallets) {
        lPendingWallets.append(BitcoinWallet(w.toByteArray()));
    }

    mAccountSettings->remove(darkWalletPendingSetting());
    mAccountSettings->sync();

    return lPendingWallets;
}

void UserAccount::createNewBrightWallet()
{
    QList<QVariant> lWalletListToSave = mAccountSettings->value(brightWalletsSetting()).toList();

    BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(currentChain());
    QString lWalletId = QString::number(QDateTime::currentMSecsSinceEpoch()).append(lNewWallet.address());

    lNewWallet.setValue("0");
    lNewWallet.setWalletId(lWalletId);
    lNewWallet.setOwner(mUsername);
    lNewWallet.setIsMicroWallet(false);

    mBrightWallet.append(lNewWallet);
    mAllWallets.insert(lNewWallet.walletId());
    lWalletListToSave.append(lNewWallet.toData());

    mAccountSettings->setValue(brightWalletsSetting(), lWalletListToSave);
    mAccountSettings->sync();
}

void UserAccount::addMicroWallet(BitcoinWallet iWallet)
{
    BitcoinWallet lWallet = iWallet;
    if (!mAllWallets.contains(lWallet.walletId())) {
        lWallet.setOwner(mUsername);
        lWallet.setIsMicroWallet(true);
        mDarkWallet.append(lWallet);
        mDarkBalance = mDarkBalance + lWallet.value();
        mAllWallets.insert(lWallet.walletId());

        sortWallet(true);

        QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();
        lWalletList.append(lWallet.toData());

        mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
        mAccountSettings->sync();

        emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
    }
}

void UserAccount::addBrightWallet(BitcoinWallet iWallet)
{
    BitcoinWallet lWallet = iWallet;
    if (!mAllWallets.contains(lWallet.walletId())) {
        lWallet.setOwner(mUsername);
        lWallet.setIsMicroWallet(false);
        mBrightWallet.append(lWallet);
        mBrightBalance = mBrightBalance + lWallet.value();
        mAllWallets.insert(lWallet.walletId());

        QList<QVariant> lWalletList = mAccountSettings->value(brightWalletsSetting()).toList();
        lWalletList.append(lWallet.toData());

        mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
        mAccountSettings->sync();

        emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
    }
}

void UserAccount::removeMicroWallet(BitcoinWallet iWallet)
{
    QList<QVariant> lWalletList;
    mDarkBalance = QStringMath();

    int lIndexToRemove = 0;
    int lCurrentIndex = 0;

    // NEW
    for (auto w : mDarkWallet) {
        if (mDarkWallet.at(lCurrentIndex).walletId() == iWallet.walletId()) {
            lIndexToRemove = lCurrentIndex;
        } else {
            lWalletList.append(w.toData());
            mDarkBalance = mDarkBalance + w.value();
        }
        lCurrentIndex++;
    }

    mDarkWallet.removeAt(lIndexToRemove);
    mAllWallets.remove(iWallet.walletId());

    mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
    mAccountSettings->sync();
}

void UserAccount::setBrightWallets(QList<BitcoinWallet> iWallets)
{
    for (auto w : iWallets) {
        mAllWallets.remove(w.walletId());
    }

    mBrightWallet.clear();
    mBrightWallet = iWallets;
    mBrightBalance = QStringMath();

    QList<QVariant> lWalletList;

    for (BitcoinWallet &w : mBrightWallet) {
        w.setOwner(mUsername);
        lWalletList.append(w.toData());
        mBrightBalance = mBrightBalance + w.value();
        mAllWallets.insert(w.walletId());
    }

    mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
    mAccountSettings->sync();

    emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
}

void UserAccount::setDarkWallets(QList<BitcoinWallet> iWallets)
{
    for (auto w : mDarkWallet) {
        mAllWallets.remove(w.walletId());
    }

    mDarkWallet.clear();
    mDarkWallet = iWallets;
    sortWallet(true);

    mDarkBalance = QStringMath();

    QList<QVariant> lWalletList;

    for (auto w : mDarkWallet) {
        mAllWallets.insert(w.walletId());
        lWalletList.append(w.toData());
        mDarkBalance = mDarkBalance + w.value();
    }

    mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
    mAccountSettings->sync();

    emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
}

void UserAccount::setBrightPendingBalance(QStringMath iValue)
{
    mBrightPendingBalance = iValue;

    mAccountSettings->setValue(brightPendingBalanceSetting(), mBrightPendingBalance.toString());
    mAccountSettings->sync();
}

void UserAccount::removeBrightWallets(QString iAmount)
{
    QStringMath lValue = iAmount;
    QList<QVariant> lWallets;

    for (BitcoinWallet &w : mBrightWallet) {
        if (QStringMath(w.value()) <= lValue) {
            lValue = lValue - w.value();
            mBrightBalance = mBrightBalance - w.value();
            w.setValue("0.0");
        } else {
            w.setValue((QStringMath(w.value()) - lValue).toString());
            mBrightBalance = mBrightBalance - lValue;
            lValue = QStringMath();
        }
        lWallets.append(w.toData());
    }

    mAccountSettings->setValue(brightWalletsSetting(), lWallets);
    mAccountSettings->sync();

    updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
}

void UserAccount::clearAllSavedData()
{
    QStringList lKeys = mAccountSettings->allKeys();
    for (auto k : lKeys) {
        mAccountSettings->remove(k);
    }
    mAccountSettings->deleteLater();
    mBitcoinBlockchainInterface->deleteLater();
    emit clearAllSavedDataComplete();
}

bool UserAccount::backupAccount(QString iEmail)
{
    QByteArray      lAttachmentData;
    QJsonObject     lJson;
    QJsonArray      lDarkWalletArray;
    QJsonArray      lBrightWalletArray;
    QJsonArray      lNotificationsArray;

    for (auto w : getDarkWallets()) {
        lDarkWalletArray.push_back(QString(w.toData()));
    }

    for (auto w : getBrightWallets()) {
        lBrightWalletArray.push_back(QString(w.toData()));
    }

    for (auto n : getNotifications()) {
        lNotificationsArray.push_back(n);
    }

    lJson.insert("Action", "ImportAccount");
    lJson.insert("Notes", QString("Account backup for %1").arg(mUsername));
    lJson.insert("Username", getUsername());
    lJson.insert("Email", getEmail());
    lJson.insert("PrivateKey", QVariant(getPrivateKey()).toJsonValue());
    lJson.insert("PublicKey", QVariant(getPublicKey()).toJsonValue());
    lJson.insert("BrightWallets", lBrightWalletArray);
    lJson.insert("DarkWallets", lDarkWalletArray);
    lJson.insert("Notifications", lNotificationsArray);

    QJsonDocument lJsonDoc (lJson);
    lAttachmentData = lJsonDoc.toJson();

    SmtpClient lClient ("smtp.gmail.com", 465, SmtpClient::SslConnection);

    MimeMessage lMessage;
    EmailAddress lFromEmail("admin@threebx.com", "admin@threebx.com");
    EmailAddress lToEmail(iEmail, iEmail);

    MimeAttachment lAttachment(lAttachmentData, "accountBackup.threshodl");

    MimeHtml lHtmlBody(QString("<html>"
                               "<h3>Threshodl Dark Account Backup</h3>"
                               "<br>"
                               "<p>Hello %1! Here is your account backup. To restore your account to this backup, open the attachmemnt "
                               "with the Threshodl app. This backup will overwrite your current app settings/information. Thanks!</p>"
                               "<br><br>"
                               "Sent at %2 on %3"
                               "<br><br>"
                               "</html>").arg(mUsername).arg(QTime::currentTime().toString()).arg(QDate::currentDate().toString(myDateFormat())));

    lMessage.setSubject(QString("Threshodl - %1 Account Backup!").arg(mUsername));
    lMessage.addRecipient(&lToEmail);
    lMessage.setSender(&lFromEmail);

    lMessage.addPart(&lHtmlBody);
    lMessage.addPart(&lAttachment);

    if(lClient.connectToHost()) {
        if (lClient.login("admin@threebx.com", "H0lyP33rP@1d", SmtpClient::AuthLogin)) {
            if (lClient.sendMail(lMessage)) {
                //success
                qDebug() << "Success";
            } else {
                //failure
                qDebug() << "Failure";
                return false;
            }
        } else {
            qDebug() << "AUTH failed for smtp client";
            return false;
        }
    } else {
        qDebug() << "Failed to connect to smtp host";
        return false;
    }

    return true;
}

bool UserAccount::importAccount(QByteArray iData)
{
    mBrightBalance          = QStringMath();
    mBrightPendingBalance   = QStringMath();
    mDarkBalance            = QStringMath();
    mBrightWallet.clear();
    mDarkWallet.clear();
    mAllWallets.clear();

    QStringList lKeys = mAccountSettings->allKeys();
    for (auto k : lKeys) {
        mAccountSettings->remove(k);
    }

    QJsonDocument lDoc = QJsonDocument::fromJson(iData);
    QJsonObject lObject = lDoc.object();
    QJsonArray lBrightArray = lObject["BrightWallets"].toArray();
    QJsonArray lDarkArray = lObject["DarkWallets"].toArray();
    QJsonArray lNotificationArray = lObject["Notifications"].toArray();

    for (auto w : lBrightArray) {
        BitcoinWallet lNewWallet = BitcoinWallet(w.toVariant().toByteArray());
        addBrightWallet(lNewWallet);
    }

    for (auto w : lDarkArray) {
        BitcoinWallet lNewWallet = BitcoinWallet(w.toVariant().toByteArray());
        addMicroWallet(lNewWallet);
    }

    for (int i = 0; i < lNotificationArray.size(); i+=2) {
        addNotification(lNotificationArray.at(i).toString(),lNotificationArray.at(i+1).toString());
    }

    createNewAccount(lObject["Username"].toString(), lObject["PrivateKey"].toVariant().toByteArray(), lObject["PublicKey"].toVariant().toByteArray());
    mEmail = lObject["Email"].toString();

    updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());

    return true;
}

void UserAccount::updateFromBrightComplete(bool iSuccess)
{
    emit updateBrightBalanceComplete(iSuccess);
}

bool UserAccount::isNewAccount()
{
    if (mUsername == "") {
        return true;
    }
    return false;
}

bool UserAccount::accountContainsWallet(QString iWalletId)
{
    return mAllWallets.contains(iWalletId);
}

void UserAccount::updateBrightBalanceFromBlockchain()
{
    mBitcoinBlockchainInterface->updateBrightWalletBalances();

//    if (mBrightBalance == mBrightPendingBalance) {
    emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
//    } else {
//        emit updateBalancesForViews(mBrightPendingBalance.toString(), mDarkBalance.toString());
//    }

    emit updateBrightBalanceComplete(true);
}

bool UserAccount::sendBrightTransaction(QString iToAddress, QString iAmount)
{
    QStringList         lTxids;
    QStringList         lValues;
    QStringList         lVouts;
    QList<QByteArray>   lPrivateKeys;
    QByteArray          lRawTransaction = QByteArray();
    QByteArray          lSignedHex = QByteArray();

    BitcoinWallet       lTempToWallet;

    lTempToWallet.setAddress(iToAddress.toUtf8());
    lTempToWallet.setValue(QStringMath(iAmount).toString());
    lTempToWallet.setChain(currentChain());

    if (mBitcoinBlockchainInterface->getUnspentTransactions(mBrightWallet, lTxids, lValues, lVouts, lPrivateKeys)) {
        // Estimate fee and create raw transaction
        QString lMinerFee = mBitcoinBlockchainInterface->estimateMinerFee(lTxids.count(), 2, false);
        lTempToWallet.setValue((QStringMath(lTempToWallet.value()) - lMinerFee).toString()); // correct output for fee

        lRawTransaction = mBitcoinBlockchainInterface->createBitcoinTransaction(mBrightWallet, QList<BitcoinWallet>() << lTempToWallet, lMinerFee, lPrivateKeys);
        if (lRawTransaction != QByteArray()) {
            // sign transaction
            if (mBitcoinBlockchainInterface->signRawTransaction(lRawTransaction, lSignedHex, lPrivateKeys)) {
                qDebug() << "Transaction signed!";
                if (mBitcoinBlockchainInterface->sendRawTransaction(lSignedHex)) {
                    qDebug() << "Transaction sent!";
                    mBrightBalance = mBrightBalance - iAmount;
                    updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
                    return true;
                }
            } else {
                qDebug() << "Failed to sign transaction";
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }

    return true;
}

void UserAccount::loadFromSettings()
{
    if (mAccountSettings->contains(usernameSetting())) {
        mUsername = mAccountSettings->value(usernameSetting()).toString();
        mPrivateKey = mAccountSettings->value(privateKeySetting()).toByteArray();
        mPublicKey = mAccountSettings->value(publicKeySetting()).toByteArray();
    } else {
        mUsername = "";
    }

    if (mAccountSettings->contains(notificationsSetting())) {
        QList<QVariant> lNotifications = mAccountSettings->value(notificationsSetting()).toList();
        setNotifications(lNotifications);
    }

    mEmail = mAccountSettings->value(emailSetting()).toString();
    mBrightPendingBalance = mAccountSettings->value(brightPendingBalanceSetting()).toString();

    if (!mAccountSettings->contains(brightWalletsSetting())) {
        //create new wallet
        mBrightWallet.clear();
        createNewBrightWallet();


#if 0
        ////////////////////////
        ////////////////////////
        // For testing only.....
        mBrightWallet[0].setValue("10.00");
        mBrightBalance = mBrightWallet[0].value();
        // .....................
        ////////////////////////
        ////////////////////////
#endif


        QVariantList lWalletList;
        lWalletList.append(getBrightWallet().toData());
        mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
        mAccountSettings->sync();

    } else {
        //pull wallet from storage
        QList<QVariant> lWalletList = mAccountSettings->value(brightWalletsSetting()).toList();
        mBrightBalance = QStringMath();
        mBrightWallet.clear();
        for (auto w : lWalletList) {
            BitcoinWallet lNewWallet = BitcoinWallet(w.toByteArray());
            mBrightWallet.append(lNewWallet);
            mBrightBalance = mBrightBalance + lNewWallet.value();
        }
    }

    mDarkBalance = QStringMath();
    if (mAccountSettings->contains(darkWalletsSetting())) {
        mDarkWallet.clear();

        QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();

        for (auto w : lWalletList) {
            BitcoinWallet lNewWallet = BitcoinWallet(w.toByteArray());
            mDarkWallet.append(lNewWallet);
            mDarkBalance = mDarkBalance + lNewWallet.value();
        }

        sortWallet(true);

    } else {
        mDarkWallet.clear();

#if 0
        ////////////////////////
        ////////////////////////
        // For testing only.....
        QList<BitcoinWallet> lNewWallets;

        for (int i = 0; i <20; i++) {
            BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(currentChain());
            QString lNewWalletId = QString::number(QDateTime::currentMSecsSinceEpoch()).append(lNewWallet.address());
            lNewWallet.setOwner(mUsername);
            lNewWallet.setValue("0.1");
            lNewWallet.setIsMicroWallet(true);
            lNewWallet.setWalletId(lNewWallet.address());

            lNewWallets.push_back(lNewWallet);
        }

        setDarkWallets(lNewWallets);

        // .....................
        ////////////////////////
        ////////////////////////
#endif

    }

    emit updateBalancesForViews(mBrightBalance.toString(), mDarkBalance.toString());
}

bool UserAccount::greaterThanWallet(BitcoinWallet iLHS, BitcoinWallet iRHS)
{
    QStringMath lLeft (iLHS.value());
    QStringMath lRight (iRHS.value());

    return lLeft > lRight;
}

void UserAccount::sortWallet(bool iSortDark)
{
    if (iSortDark) {
        if (mDarkWallet.count() > 1 && mDarkWallet.count() != 0) {
            std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
                QStringMath lLeft = QStringMath(lhs.value());
                QStringMath lRight = QStringMath(rhs.value());
                return lLeft > lRight;
            });
        }
    } else {
        if (mBrightWallet.count() > 1 && mBrightWallet.count() != 0) {
            std::sort(mBrightWallet.begin(), mBrightWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
                QStringMath lLeft = QStringMath(lhs.value());
                QStringMath lRight = QStringMath(rhs.value());
                return lLeft > lRight;
            });
        }
    }
}













