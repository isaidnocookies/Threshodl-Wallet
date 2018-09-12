#include "myqsettingsmanager.h"
#include "walletaccount.h"

#include <QCoreApplication>

MyQSettingsManager::MyQSettingsManager(QObject *parent) : QObject(parent)
{
    QCoreApplication::setOrganizationDomain(AppInfo::appDomain());
    QCoreApplication::setApplicationName(AppInfo::appName());
    QCoreApplication::setOrganizationName(AppInfo::appOrganization());

    mAccountData = new QSettings;
}

bool MyQSettingsManager::userAccountExists()
{
    if (mAccountData->contains(DataKeys::usernameDataKey())) {
        return true;
    } else {
        return false;
    }
}

void MyQSettingsManager::saveUsername(QString iUsername)
{
    mAccountData->setValue(DataKeys::usernameDataKey(), iUsername);
    mAccountData->sync();
}

void MyQSettingsManager::saveRecoverySeed(QString iSeed)
{
    mAccountData->setValue(DataKeys::recoverySeedDataKey(), iSeed);
    mAccountData->sync();
}

QString MyQSettingsManager::getRecoverySeed()
{
    if (mAccountData->contains(DataKeys::recoverySeedDataKey())) {
        return mAccountData->value(DataKeys::recoverySeedDataKey()).toString();
    }
    return "";
}

void MyQSettingsManager::savePublicAndPrivateKeys(QString iPublicKey, QString iPrivateKey)
{
    mAccountData->setValue(DataKeys::publicKeyDataKey(), iPublicKey);
    mAccountData->setValue(DataKeys::privateKeyDataKey(), iPrivateKey);

    mAccountData->sync();
}

void MyQSettingsManager::usernameAndKeys(QString &oUsername, QString &oPublicKey, QString &oPrivateKey)
{
    oUsername = mAccountData->value(DataKeys::usernameDataKey()).toString();
    oPublicKey = mAccountData->value(DataKeys::publicKeyDataKey()).toString();
    oPrivateKey = mAccountData->value(DataKeys::privateKeyDataKey()).toString();
}

void MyQSettingsManager::saveWallet(QByteArray iWalletData, QString iShortname, bool isDark)
{
    if (!isDark) {
        mAccountData->beginGroup(iShortname);

        QList<QVariant> lWallets = mAccountData->value(DataKeys::walletsDataKey()).toList();
        lWallets.append(iWalletData);
        mAccountData->setValue(DataKeys::walletsDataKey(), lWallets);

        mAccountData->endGroup();
        mAccountData->sync();
    } else {
        mAccountData->beginGroup("d" + iShortname);

        QList<QVariant> lWallets = mAccountData->value(DataKeys::walletsDataKey()).toList();
        lWallets.append(iWalletData);
        mAccountData->setValue(DataKeys::walletsDataKey(), lWallets);

        mAccountData->endGroup();
        mAccountData->sync();
    }
}

void MyQSettingsManager::saveWalletAccount(QString iShortName, QString iLongName, CryptoNetwork iChainType)
{
    mAccountData->beginGroup(iShortName);

    mAccountData->setValue(DataKeys::shortNameDataKey(), iShortName);
    mAccountData->setValue(DataKeys::longNameDataKey(), iLongName);
    mAccountData->setValue(DataKeys::chainDataKey(), static_cast<int>(iChainType));

    mAccountData->endGroup();
    mAccountData->sync();
}

void MyQSettingsManager::savePasscode(QString iPasscode)
{
    mAccountData->setValue(DataKeys::passcodeDataKey(), iPasscode);
    mAccountData->sync();
}

QString MyQSettingsManager::getPasscode()
{
    if (mAccountData->contains(DataKeys::passcodeDataKey())) {
        return mAccountData->value(DataKeys::passcodeDataKey()).toString();
    } else {
        return "";
    }
}

void MyQSettingsManager::saveEmailAddress(QString iEmail)
{
    mAccountData->setValue(DataKeys::emailDataKey(), iEmail);
    mAccountData->sync();
}

QString MyQSettingsManager::getEmail()
{
    if (mAccountData->contains(DataKeys::emailDataKey())) {
        return mAccountData->value(DataKeys::emailDataKey()).toString();
    } else {
        return "";
    }
}

void MyQSettingsManager::getBrightWalletAccounts(QList<WalletAccount> &oWalletAccounts)
{
    for (int i = 0; i < AppWallets::walletNames().keys().size(); i++) {
        mAccountData->beginGroup(AppWallets::walletNames().keys().at(i));

        QString lShortName = mAccountData->value(DataKeys::shortNameDataKey()).toString();
        QString lLongName = mAccountData->value(DataKeys::longNameDataKey()).toString();
        QString lSeed = mAccountData->value(DataKeys::recoverySeedDataKey()).toString();
        CryptoNetwork lChain = static_cast<CryptoNetwork>(mAccountData->value(DataKeys::chainDataKey()).toInt());
        WalletAccount lNewWalletAccount(lShortName, lLongName, lChain);
        QList<QVariant> lWallets = mAccountData->value(DataKeys::walletsDataKey()).toList();

        lNewWalletAccount.setExchangeCurrency(mAccountData->value(DataKeys::exchangeCurrencyDataKey()).toString());
        lNewWalletAccount.setMarketValue(mAccountData->value(DataKeys::marketValueDataKey()).toString());

        QStringMath lConfirmedBalance;
        for (auto w : lWallets) {
            CryptoWallet lW = CryptoWallet(w.toByteArray());
            lConfirmedBalance = lConfirmedBalance + lW.value();
            lNewWalletAccount.addWallet(lW);
        }
        lNewWalletAccount.setConfirmedBalance(lConfirmedBalance.toString());
        oWalletAccounts.append(lNewWalletAccount);

        mAccountData->endGroup();
    }
}

void MyQSettingsManager::getDarkWalletAccounts(QList<WalletAccount> &oDarkWalletAccounts)
{
    //TODO
}

void MyQSettingsManager::getWalletBalance(QString iShortname, QString &oConfirmed, QString &oUnconfirmed)
{
    QString lBalance;
    QString lUnconfirmedBalance;

    mAccountData->beginGroup(iShortname);

    if (mAccountData->contains(DataKeys::walletConfirmedBalanceDataKey())) {
        lBalance = mAccountData->value(DataKeys::walletConfirmedBalanceDataKey()).toString();
        lUnconfirmedBalance = mAccountData->value(DataKeys::walletUnconfirmedBalanceDataKey()).toString();
    } else {
        lBalance = "";
        lUnconfirmedBalance = "";
    }

    mAccountData->endGroup();

    oConfirmed = lBalance;
    oUnconfirmed = lUnconfirmedBalance;
}

void MyQSettingsManager::saveWalletBalance(QString iShortname, QString iConfirmedBalance, QString iUnconfirmedBalance)
{
    mAccountData->beginGroup(iShortname);
    mAccountData->setValue(DataKeys::walletConfirmedBalanceDataKey(), iConfirmedBalance);
    mAccountData->setValue(DataKeys::walletUnconfirmedBalanceDataKey(), iUnconfirmedBalance);
    mAccountData->endGroup();
    mAccountData->sync();
}
