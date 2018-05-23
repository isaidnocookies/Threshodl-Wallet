#include "useraccount.h"

#include <QDebug>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";

    mDataManager = new QSettingsManager;

    mBtcConfirmedBalance = "0.0";
    mBtcUnconfirmedBalance = "0.0";

    mBrightBitcoinConfirmedBalance = "0.0";
    mBrightBitcoinUnconfirmedBalance = "0.0";
    mDarkBitcoinConfirmedBalance = "0.0";
    mDarkBitcoinUnconfirmedBalance = "0.0";

    mEthConfirmedBalance = "0.0";
    mEthUnconfirmedBalance = "0.0";

    mLtcConfirmedBalance = "0.0";
    mLtcUnconfirmedBalance = "0.0";

    loadAccountFromSettings();
}

bool UserAccount::exists()
{
    return mDataManager->userAccountExists();
}

void UserAccount::createNewAccount(QString iUsername)
{
    mCreateUsername = new CreateUsername;
    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    setWaiting(true);
    mCreateUsername->create(iUsername);
    setCurrentErrorString("");
}

QString UserAccount::getTotalBalance(QString iCurrency)
{
    // use marketValues for this..
    if (iCurrency == "USD") {
        return "0.00";
    } else {
        return "0.00";
    }
}

QString UserAccount::getBitcoinBalance(QString iType, bool iConfirmed)
{
    if (iType == "Dark") {
        if (iConfirmed) {
            return mDarkBitcoinConfirmedBalance;
        } else {
            return mDarkBitcoinUnconfirmedBalance;
        }
    } else {
        if (iConfirmed) {
            return mBrightBitcoinConfirmedBalance;
        } else {
            return mBrightBitcoinUnconfirmedBalance;
        }
    }
}

bool UserAccount::isWalletConfirmed(QString iCurrency, QString iWalletType)
{
    if (iCurrency == "BTC") {
        if (iWalletType == "Dark") {
            if (QStringMath(mDarkBitcoinUnconfirmedBalance) == "0.0")
                return true;
            return false;
        } else if (iWalletType == "Bright") {
            if (QStringMath(mBrightBitcoinUnconfirmedBalance) == "0.0")
                return true;
            return false;
        } else {
            //both
            if (QStringMath(mBrightBitcoinConfirmedBalance) == "0.0" && QStringMath(mDarkBitcoinUnconfirmedBalance) == "0.0")
                return true;
            return false;
        }
    } else if (iCurrency == "ETH") {
        if (QStringMath(mEthUnconfirmedBalance) == "0.0")
            return true;
        return false;
    } else if (iCurrency == "LTC") {
        if (QStringMath(mLtcUnconfirmedBalance) == "0.0")
            return true;
        return false;
    }
    return false;
}

void UserAccount::setUsername(QString iUsername)
{
    if (iUsername == mUsername) { return; }

    mUsername = iUsername;
    emit usernameChanged();
}

void UserAccount::setWaiting(bool iWaiting)
{
    if (iWaiting == mWaiting) { return; }

    mWaiting = iWaiting;
    emit waitingChanged();
}

void UserAccount::setCurrentErrorString(QString iCurrentErrorString)
{
    if (iCurrentErrorString == mCurrentErrorString) { return; }

    mCurrentErrorString = iCurrentErrorString;
    emit currentErrorStringChanged();
}

void UserAccount::publicAndPrivateKeys(QByteArray &oPublicKey, QByteArray &oPrivateKey)
{
    oPublicKey = mPublicKey;
    oPrivateKey = mPrivateKey;
}

void UserAccount::setPublicAndPrivateKeys(QByteArray iPublicKey, QByteArray iPrivateKey)
{
    mPublicKey = iPublicKey;
    mPrivateKey = iPrivateKey;
}

void UserAccount::setBitcoinConfirmedBalance(QString iBtcConfirmed)
{
    if (iBtcConfirmed == mBtcConfirmedBalance) { return; }

    mBtcConfirmedBalance = iBtcConfirmed;
    emit bitcoinConfirmedBalanceChanged();
    emit cryptoConfirmedBalanceChanged();
}

void UserAccount::setBitcoinUnconfirmedBalance(QString iBtcUnconfirmed)
{
    if (iBtcUnconfirmed == mBtcUnconfirmedBalance) { return; }

    mBtcUnconfirmedBalance = iBtcUnconfirmed;
    emit bitcoinUnconfirmedBalanceChanged();
}

void UserAccount::setEthereumConfirmedBalance(QString iEthConfirmed)
{
    if (iEthConfirmed == mEthConfirmedBalance) { return; }

    mEthConfirmedBalance = iEthConfirmed;
    emit ethereumConfirmedBalanceChanged();
    emit cryptoConfirmedBalanceChanged();
}

void UserAccount::setEthereumUnconfirmedBalance(QString iEthUnconfirmed)
{
    if (iEthUnconfirmed == mEthUnconfirmedBalance) { return; }

    mEthUnconfirmedBalance = iEthUnconfirmed;
    emit ethereumUnconfirmedBalanceChanged();
}

void UserAccount::setLitecoinConfirmedBalance(QString iLtcConfirmed)
{
    if (iLtcConfirmed == mLtcConfirmedBalance) { return; }

    mLtcConfirmedBalance = iLtcConfirmed;
    emit litecoinConfirmedBalanceChanged();
    emit cryptoConfirmedBalanceChanged();
}

void UserAccount::setLitecoinUnconfirmedBalance(QString iLtcUnconfirmed)
{
    if (iLtcUnconfirmed == mLtcUnconfirmedBalance) { return; }

    mLtcUnconfirmedBalance = iLtcUnconfirmed;
    emit litecoinUnconfirmedBalanceChanged();
}

void UserAccount::usernameCreated(bool iSuccess, QString iUsername, QByteArray iPublicKey, QByteArray iPrivateKey)
{
    if (iSuccess) {
        setUsername(iUsername);
        setPublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveUsername(iUsername);
        mDataManager->savePublicAndPrivateKeys(iPublicKey, iPrivateKey);
        setCurrentErrorString("");

        // create wallets for each coin...
    } else {
        setCurrentErrorString("Failed to create account");
        qDebug() << "Error creating account...";
    }
    setWaiting(false);
}

void UserAccount::loadAccountFromSettings()
{
    if (mDataManager->userAccountExists()) {
        QString lUsername;
        QByteArray lPub;
        QByteArray lPriv;

        mDataManager->usernameAndKeys(lUsername, lPub, lPriv);

        setUsername(lUsername);
        setPublicAndPrivateKeys(lPub, lPriv);
    } else {
        qDebug() << "No account created";
    }
}

void UserAccount::createCryptoWallets()
{

}
