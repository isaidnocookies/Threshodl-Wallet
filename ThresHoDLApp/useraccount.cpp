#include "useraccount.h"
#include "globalsandstyle.h"
#include "bitcoinwallet.h"

#include <QDebug>
#include <QCoreApplication>

UserAccount::UserAccount(QObject *parent)
{
    Q_UNUSED(parent)

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
    BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(BitcoinWallet::ChainType::TestNet);
    QString lWalletId = QString::number(QDateTime::currentMSecsSinceEpoch()).append(lNewWallet.address());

    lNewWallet.setValue("0");
    lNewWallet.setWalletId(lWalletId);

    mBrightWallet.append(lNewWallet);
}

void UserAccount::addMicroWallet(BitcoinWallet iWallet)
{
    if (iWallet.chain() != BitcoinWallet::ChainType::None && !mAllWallets.contains(iWallet.walletId())) {
        mDarkWallet.append(iWallet);
        mDarkBalance += iWallet.value().toDouble();
        mAllWallets.insert(iWallet.walletId());

        std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
                                    return lhs.value().toDouble() < rhs.value().toDouble();
                                });

        QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();
        lWalletList.append(iWallet.toData());

        mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
        mAccountSettings->sync();

        emit updateBalancesForMainWindow(mBrightBalance, mDarkBalance);
    }
}

void UserAccount::addBrightWallet(BitcoinWallet iWallet)
{
    if (iWallet.chain() != BitcoinWallet::ChainType::None && !mAllWallets.contains(iWallet.walletId())) {
        mBrightWallet.append(iWallet);
        mBrightBalance += iWallet.value().toDouble();
        mAllWallets.insert(iWallet.walletId());

        QList<QVariant> lWalletList = mAccountSettings->value(brightWalletsSetting()).toList();
        lWalletList.append(iWallet.toData());

        mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
        mAccountSettings->sync();

        emit updateBalancesForMainWindow(mBrightBalance, mDarkBalance);
    }
}

void UserAccount::setBrightWallets(QList<BitcoinWallet> iWallets)
{
    mBrightWallet.clear();
    mBrightWallet = iWallets;
    mBrightBalance = 0;

    QList<QVariant> lWalletList;

    for (auto w : iWallets) {
        lWalletList.append(w.toData());
        mBrightBalance += w.value().toDouble();
    }

    mAccountSettings->setValue(brightWalletsSetting(), lWalletList);
    mAccountSettings->sync();

    emit updateBalancesForMainWindow(mBrightBalance, mDarkBalance);
}

void UserAccount::setDarkWallets(QList<BitcoinWallet> iWallets)
{
    mDarkWallet.clear();
    mDarkWallet = iWallets;

    std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
        return lhs.value().toDouble() < rhs.value().toDouble();
    });

    mDarkBalance = 0;

    QList<QVariant> lWalletList;

    for (auto w : iWallets) {
        lWalletList.append(w.toData());
        mDarkBalance += w.value().toDouble();
    }

    mAccountSettings->setValue(darkWalletsSetting(), lWalletList);
    mAccountSettings->sync();

    emit updateBalancesForMainWindow(mBrightBalance, mDarkBalance);
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

        mBrightWallet[0].setValue("10.00");
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
        mBrightBalance = 0;
        mBrightWallet.clear();
        for (auto w : lWalletList) {
            BitcoinWallet lNewWallet = BitcoinWallet(w.toByteArray());
            mBrightWallet.append(lNewWallet);
            mBrightBalance += lNewWallet.value().toDouble();
        }
    }

    mDarkBalance = 0;
    if (mAccountSettings->contains(darkWalletsSetting())) {
        mDarkWallet.clear();

        QList<QVariant> lWalletList = mAccountSettings->value(darkWalletsSetting()).toList();

        for (auto w : lWalletList) {
            BitcoinWallet lNewWallet = BitcoinWallet(w.toByteArray());
            mDarkWallet.append(lNewWallet);
            mDarkBalance += lNewWallet.value().toDouble();
        }

        std::sort(mDarkWallet.begin(), mDarkWallet.end(), [] (const BitcoinWallet& lhs, const BitcoinWallet& rhs) {
            return lhs.value().toDouble() < rhs.value().toDouble();
        });
    } else {
        mDarkWallet.clear();

#if 1
        ////////////////////////
        ////////////////////////
        // For testing only.....
        QList<BitcoinWallet> lNewWallets;

        for (int i = 0; i <20; i++) {
            BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(BitcoinWallet::ChainType::TestNet);
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

    emit updateBalancesForMainWindow(mBrightBalance, mDarkBalance);
}

bool UserAccount::greaterThanWallet(BitcoinWallet iLHS, BitcoinWallet iRHS)
{
    if (iLHS.value() == iRHS.value()) {
        return iLHS.value() > iRHS.value();
    } else {
        return iLHS.value() > iRHS.value();
    }
}













