#include "useraccount.h"
#include "globalsandstyle.h"
#include "bitcoinwallet.h"

#include <QDebug>
#include <QCoreApplication>

UserAccount::UserAccount()
{
    QCoreApplication::setOrganizationDomain(theDomain());
    QCoreApplication::setApplicationName(theApplicationName());
    QCoreApplication::setOrganizationName(theOrganization());

    mAccountSettings = new QSettings;

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
        return BitcoinWallet::createNewBitcoinWallet(BitcoinWallet::ChainType::None);
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

void UserAccount::setNotifications(QList<QVariant> lNotifications)
{
    mNotifications.clear();
    int lCount = lNotifications.size();

    for (int i = 0; i < lCount; i+=2) {
        addNotification(lNotifications.at(i).toString(), lNotifications.at(i+1).toString());
    }
}

void UserAccount::setAddresses(QString iEmail, QString iThreshodlAddress)
{
    mEmail = iEmail;

    mAccountSettings->setValue(addressSetting(), iThreshodlAddress);
    mAccountSettings->setValue(emailSetting(), iEmail);

    mAccountSettings->sync();
}

void UserAccount::createNewBrightWallet()
{
    BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(BitcoinWallet::ChainType::TestNet);
    lNewWallet.setValue("0");

    mBrightWallet.append(lNewWallet);
}

void UserAccount::addMicroWallet(BitcoinWallet iWallet)
{
    if (iWallet.chain() != BitcoinWallet::ChainType::None) {
        mDarkWallet.append(iWallet);
        mDarkBalance += iWallet.value().toDouble();
    }

    std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
                                return lhs.value().toDouble() < rhs.value().toDouble();
                            });

    QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();
    lWalletList.append(iWallet.toData());

    mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
    mAccountSettings->sync();
}

void UserAccount::addBrightWallet(BitcoinWallet iWallet)
{
    if (iWallet.chain() != BitcoinWallet::ChainType::None) {
        mBrightWallet.append(iWallet);
    }

    QList<QVariant> lWalletList = mAccountSettings->value(brightWalletsSetting()).toList();
    lWalletList.append(iWallet.toData());

    mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
    mAccountSettings->sync();
}

void UserAccount::setBrightWallets(QList<BitcoinWallet> iWallets)
{
    mBrightWallet.clear();
    mBrightWallet = iWallets;

    QList<QVariant> lWalletList;

    for (auto w : iWallets) {
        lWalletList.append(w.toData());
    }

    mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
    mAccountSettings->sync();
}

void UserAccount::setDarkWallets(QList<BitcoinWallet> iWallets)
{
    mDarkWallet.clear();
    mDarkWallet = iWallets;

    std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
        return lhs.value().toDouble() < rhs.value().toDouble();
    });

    QList<QVariant> lWalletList;

    for (auto w : iWallets) {
        lWalletList.append(w.toData());
    }

    mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
    mAccountSettings->sync();
}

bool UserAccount::isNewAccount()
{
    if (mUsername == "") {
        return true;
    }
    return false;
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

    if (!mAccountSettings->contains(brightWalletsSetting())) {
        //create new wallet
        mBrightWallet.clear();
        createNewBrightWallet();


        ////////////////////////
        ////////////////////////
        // For testing only.....

        mBrightWallet[0].setValue("10.0013");
        mBrightBalance = mBrightWallet[0].value().toDouble();

        // .....................
        ////////////////////////
        ////////////////////////


        QVariantList lWalletList;
        lWalletList.append(getBrightWallet().toData());
        mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
        mAccountSettings->sync();
    } else {
        //pull wallet from storage
        QList<QVariant> lWalletList = mAccountSettings->value(brightWalletsSetting()).toList();
        for (auto w : lWalletList) {
            addBrightWallet(BitcoinWallet(w.toByteArray()));
        }
        mBrightBalance = 0;
        for (auto w : mBrightWallet) {
            mBrightBalance += w.value().toDouble();
        }
    }

    mDarkBalance = 0;
    if (mAccountSettings->contains(darkWalletsSetting())) {
        mDarkWallet.clear();

        QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();

        for (auto w : lWalletList) {
            addMicroWallet(BitcoinWallet(w.toByteArray()));
        }

        std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
            return lhs.value().toDouble() < rhs.value().toDouble();
        });

        for (auto w : mDarkWallet) {
            mDarkBalance += w.value().toDouble();
        }
    } else {
        mDarkWallet.clear();
    }
}

bool UserAccount::greaterThanWallet(BitcoinWallet iLHS, BitcoinWallet iRHS)
{
    if (iLHS.value() == iRHS.value()) {
        return iLHS.value() > iRHS.value();
    } else {
        return iLHS.value() > iRHS.value();
    }
}













