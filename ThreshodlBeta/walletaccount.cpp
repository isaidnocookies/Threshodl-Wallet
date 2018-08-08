#include "walletaccount.h"

WalletAccount::WalletAccount()
{
}

WalletAccount::WalletAccount(QString iShortName, QString iLongName, QString iSeed, CryptoChain iChain)
{
    mShortName = iShortName;
    mLongName = iLongName;
    mChain = iChain;
    mName = name();
    mSeed = iSeed;

    WalletAccount();
}

WalletAccount::WalletAccount(QByteArray iData)
{
    QJsonParseError lError;
    QVariantMap lDataMap = QJsonDocument::fromJson(iData,&lError).toVariant().toMap();

    if (lError.error == QJsonParseError::NoError) {
        mShortName = lDataMap["shortname"].toString();
        mLongName = lDataMap["longname"].toString();
        mName = lDataMap["mName"].toString();
        mSeed = lDataMap["seed"].toString();
        mIsDark = lDataMap["isDark"].toBool();
        mConfirmedBalance = lDataMap["confirmedBalance"].toString();
        mUnconfirmedBalance = lDataMap["unconfirmedBalance"].toString();
        mChain = static_cast<CryptoChain>(lDataMap["chain"].toInt());
        mMarketValue = lDataMap["marketValue"].toString();
        mExchangeCurrency = lDataMap["exchangeCurrency"].toString();

        QList<QVariant> lWallets = lDataMap["wallets"].toList();
        mWallets.clear();

        for (auto sw : lWallets) {
            mWallets.append(GenericWallet(sw.toByteArray()));
        }
    } else {
        //failed...
    }
}

void WalletAccount::setDataManager(MyQSettingsManager *iDataMananger)
{
    mAccountData = iDataMananger;
}

QString WalletAccount::name()
{
    return QString(mIsDark ? "d" : "").append(mShortName);
}

QString WalletAccount::shortName()
{
    return mShortName;
}

QString WalletAccount::longName()
{
    return mLongName;
}

QString WalletAccount::seed()
{
    return mSeed;
}

void WalletAccount::setExchangeCurrency(QString iCurrency)
{
    if (mExchangeCurrency == iCurrency) {
        return;
    }

    mExchangeCurrency = iCurrency;
    updateMarketValue();
//    emit marketValueChanged();
}

void WalletAccount::setMarketValue(QString iCurrentValue)
{
    if (QStringMath(mMarketValue) == QStringMath(iCurrentValue)) {
        return;
    }

    mMarketValue = iCurrentValue;
}

void WalletAccount::setConfirmedBalance(QString iBalance)
{
    mConfirmedBalance = iBalance;
}

void WalletAccount::updateMarketValue()
{
    setMarketValue(mMarketValue);
}

QString WalletAccount::getBalance(bool lIsConfirmed)
{
    if (lIsConfirmed) {
        return mConfirmedBalance;
    }
    return mUnconfirmedBalance;
}

QString WalletAccount::marketValue()
{
    return mMarketValue;
}

QString WalletAccount::exchangeCurrency()
{
    return mExchangeCurrency;
}

void WalletAccount::updateBalance(QString iAddress, QString iBalance, QString iUnconfirmedBalance)
{
    for (int i = 0; i < mWallets.size(); i++) {
        if (mWallets.at(i).address() == iAddress) {
            mWallets[i].setBalance(iBalance);
            mWallets[i].setUnconfirmedBalance(iUnconfirmedBalance);
        }
    }
}

void WalletAccount::addWallet(Wallet iWallet)
{
    // check if wallet already exists...
    for (auto w : mWallets) {
        if (w.walletId() == iWallet.walletId()) {
            //wallet exists...
            return;
        }
    }
    mWallets.append(iWallet);
}

bool WalletAccount::removeWallet(QString iAddress)
{
    for (int i = 0; i < mWallets.size(); i++) {
        if (mWallets.at(i).address() == iAddress) {
            mWallets.removeAt(i);
            return true;
        }
    }
    return false;
}

bool WalletAccount::getWallet(QByteArray &oWallet, QString iAddress)
{
    for (int i = 0; i < mWallets.size(); i++) {
        if (mWallets.at(i).address() == iAddress) {
            oWallet = mWallets.at(i).toData();
            return true;
        }
    }
    return false;
}

bool WalletAccount::getWallet(Wallet &oWallet, QString iAddress)
{
    for (int i = 0; i < mWallets.size(); i++) {
        if (mWallets.at(i).address() == iAddress) {
            oWallet = mWallets.at(i);
            return true;
        }
    }
    return false;
}

QStringList WalletAccount::getWalletAddresses()
{
    QStringList lWallets;
    for ( auto w : mWallets) {
        lWallets.append(w.address());
    }
    return lWallets;
}

bool WalletAccount::getBrightAddress(QString &oAddress)
{
    oAddress = QString(mWallets.first().address());
    return true;
}

void WalletAccount::createNewBrightWallet()
{
    GenericWallet lNewWallet = GenericWallet::createWallet(mShortName, mSeed, static_cast<GenericWallet::ChainType>(mChain));
    mWallets.append(lNewWallet);
    mAccountData->saveWallet(lNewWallet.toData(), mShortName, false);
}

const QByteArray WalletAccount::toData()
{
    QVariantMap lDataMap;
    QList<QVariant> lWallets;

    lDataMap["shortname"] = mShortName;
    lDataMap["longname"] = mLongName;
    lDataMap["mName"] = mName;
    lDataMap["seed"] = mSeed;
    lDataMap["isDark"] = mIsDark;
    lDataMap["confirmedBalance"] = mConfirmedBalance;
    lDataMap["unconfirmedBalance"] = mUnconfirmedBalance;
    lDataMap["chain"] = mChain;
    lDataMap["marketValue"] = mMarketValue;
    lDataMap["exchangeCurrency"] = mExchangeCurrency;

    for (auto w : mWallets) {
        lWallets.append(w.toData());
    }

    lDataMap["wallets"] = lWallets;

    QJsonDocument lDataDoc = QJsonDocument::fromVariant(lDataMap);
    return lDataDoc.toJson(QJsonDocument::Compact);
}
