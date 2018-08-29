#include "useraccount.h"

#include <QDebug>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";
    mDataManager = new MyQSettingsManager;

    mDownloaderWaitCondition = new QWaitCondition;
    mDownloaderMutex = new QMutex;

    mMyDownloaderThread = new QThread;
    mMyDownloaderWorker = new DownloadWorker(mDownloaderWaitCondition, mDownloaderMutex);

    mMyDownloaderWorker->moveToThread(mMyDownloaderThread);

    connect(mMyDownloaderWorker, SIGNAL (error(QString)), this, SLOT (downloaderErrorString(QString)));
    connect(mMyDownloaderThread, SIGNAL (started()), mMyDownloaderWorker, SLOT (startDownloading()));
    connect(mMyDownloaderWorker, SIGNAL (finished()), mMyDownloaderThread, SLOT (quit()));
    connect(mMyDownloaderWorker, SIGNAL (finished()), mMyDownloaderWorker, SLOT (deleteLater()));
    connect(mMyDownloaderThread, SIGNAL (finished()), mMyDownloaderThread, SLOT (deleteLater()));

    connect(mMyDownloaderWorker, &DownloadWorker::walletAddressesUpdated, this, &UserAccount::walletBalancesUpdated);
    connect(mMyDownloaderWorker, &DownloadWorker::allMarketValuesUpdated, this, &UserAccount::marketValuesUpdated);
    connect(this, &UserAccount::setDownloaderAddresses, mMyDownloaderWorker, &DownloadWorker::setAddresses);

    loadAccountFromSettings();

    mMyDownloaderThread->start();
}

UserAccount::~UserAccount()
{

}

bool UserAccount::exists()
{
    return mDataManager->userAccountExists();
}

void UserAccount::createNewAccount(QString iUsername)
{
    mCreateUsername = new CreateUsername;
    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    mCreateUsername->create(iUsername);
}

void UserAccount::recoverAccount(QString iSeed)
{
    setRecoverySeed(iSeed);
    mCreateUsername = new CreateUsername;
    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    mCreateUsername->recoverAccount(iSeed);
}

QString UserAccount::getRecoverySeed()
{
    return mRecoverySeed;
}

bool UserAccount::checkPasscode(QString iPass)
{
    if (iPass == mPasscode) {
        return true;
    }
    return false;
}

void UserAccount::changePasscode(QString iNewPass)
{
    mTempPasscode = iNewPass;
}

QString UserAccount::getTempPasscode()
{
    return mTempPasscode;
}

void UserAccount::confirmPasscodeChange()
{
    mPasscode = mTempPasscode;
    mTempPasscode = "";
    mDataManager->savePasscode(mPasscode);
}

QString UserAccount::getTotalBalance(QString iCurrency)
{
    QStringMath lTotalAmount;
    if (iCurrency == "USD") {
        for (auto coinKey : AppWallets::walletNames().keys()) {
            QStringMath lCoinTotal;
            lCoinTotal = getBalanceValue(coinKey);
            lTotalAmount = lTotalAmount + lCoinTotal;
        }
        return lTotalAmount.toString();
    } else {
        return "0.00";
    }
}

QString UserAccount::getBalance(QString iShortName, bool iConfirmed)
{
    return mBrightWallets[iShortName].getBalance(iConfirmed);
}

QString UserAccount::getBalanceValue(QString iShortName, bool iConfirmed, QString iCurrency)
{
    Q_UNUSED(iCurrency) Q_UNUSED(iConfirmed)

    QString lCoinTotal;

    double lMarketValue = getMarketValue(iShortName).toDouble();
    double lBalance = getBalance(iShortName, iConfirmed).toDouble();

    lCoinTotal = QString::number(lMarketValue * lBalance);

    return lCoinTotal;
}

bool UserAccount::isWalletConfirmed(QString iShortname, QString iWalletType)
{
    bool lIsDark;
    if (iWalletType == "Dark") {
        lIsDark = true;
    } else {
        lIsDark = false;
    }

    QString lName = QString(lIsDark ? "d" : "").append(iShortname);
    if (QStringMath(mBrightWallets[lName].getBalance()) == QStringMath(mBrightWallets[lName].getBalance(false))) {
        return true;
    }

    return false;
}

void UserAccount::setUsername(QString iUsername)
{
    if (iUsername == mUsername) { return; }

    mUsername = iUsername;
    emit usernameChanged();
}

void UserAccount::setRecoverySeed(QString iSeed)
{
    mRecoverySeed = iSeed;
    mDataManager->saveRecoverySeed(iSeed);
}

QString UserAccount::getBrightAddress(QString iShortname)
{
    if (mBrightWallets.contains(iShortname)) {
        if (mBrightWallets[iShortname].hasBrightWallet()) {
            auto lAccount = mBrightWallets[iShortname];
            QString lAddress;
            if (lAccount.getBrightAddress(lAddress)) {
                return lAddress;
            }
        }
    }
    return "";
}

QString UserAccount::getMarketValue(QString iShortname, QString iCurrency)
{
    Q_UNUSED(iCurrency)

    auto lAccount = mBrightWallets[iShortname];
    QString lMarketValue = lAccount.marketValue();

    return lMarketValue;
}

QString UserAccount::getTotalWalletBalanceValue(QString iShortname, bool iConfirmed, QString iCurrency)
{
    QStringMath lWalletTotalValue;
    lWalletTotalValue = QStringMath(getBalanceValue(iShortname, iConfirmed, iCurrency)) + getBalanceValue(iShortname, iConfirmed, iCurrency);
    return lWalletTotalValue.toString();
}

void UserAccount::createRawTransaction(QString iShortname, QString toAddress, QString toAmount)
{
    QString lTxHex, lFee;
    bool lSuccess = mBrightWallets[iShortname].createRawTransaction(toAddress, toAmount, lTxHex, lFee);
    emit rawTransactionCreated(lSuccess, lTxHex, lFee);
}

void UserAccount::sendRawTransaction(QString iShortname, QString iRawTransaction)
{
    QString lTxid;
    bool lSuccess = mBrightWallets[iShortname].sendRawTransaction(iRawTransaction, lTxid);
    emit rawTransactionSent(lSuccess, lTxid);
}

QString UserAccount::sendBrightTransaction(QString iShortname, QString toAddress, QString toAmount)
{
    return mBrightWallets[iShortname].sendBrightTransaction(toAddress, toAmount);
}

QString UserAccount::createBrightWallet(QString iShortname)
{
    if (!AppWallets::walletNames().keys().contains(iShortname)) {
        return "";
    }

    QString lLongname = AppWallets::walletNames()[iShortname];

    CryptoNetwork lNetwork = CryptoNetwork::Main;
    if (iShortname.at(0) == "t") {
        lNetwork = CryptoNetwork::TestNet;
    }

    WalletAccount lWA(iShortname, lLongname, lNetwork);
    lWA.setDataManager(mDataManager);

    if (!lLongname.contains("Dark", Qt::CaseSensitive) && iShortname.at(0) != "d") {
        lWA.createNewBrightWallet(mRecoverySeed);
        QString lWalletAddress;
        if (lWA.getBrightAddress(lWalletAddress)) {
            emit setDownloaderAddresses(iShortname, QStringList() << lWalletAddress);
        }
        mBrightWallets.insert(iShortname, lWA);
        mDataManager->saveWalletAccount(iShortname, lLongname, lNetwork);
        return getBrightAddress(iShortname);
    }
    return "";
}

QVariantList UserAccount::getAllWallets()
{
    QVariantList lList;

    for (auto walletKey : mBrightWallets.keys()) {

        WalletAccount lWA = mBrightWallets[walletKey];
        auto lWallets = lWA.getWallets();
        for (auto wallet : lWallets) {
            QVariantList lWalletList;
            lWalletList.append(walletKey);
            lWalletList.append(wallet.address());
            lWalletList.append(wallet.privateKey());
            lList.append(lWalletList);
        }
    }
    return lList;
}

void UserAccount::publicAndPrivateKeys(QString &oPublicKey, QString &oPrivateKey)
{
    oPublicKey = mPublicKey;
    oPrivateKey = mPrivateKey;
}

void UserAccount::setPublicAndPrivateKeys(QString iPublicKey, QString iPrivateKey)
{
    mPublicKey = iPublicKey;
    mPrivateKey = iPrivateKey;
}

void UserAccount::setPasscode(QString iPasscode)
{
    mPasscode = iPasscode;
}

void UserAccount::usernameCreated(bool iSuccess, QString iUsername, QString iRecoverySeed, QString iPublicKey, QString iPrivateKey)
{
    if (iSuccess) {
        setUsername(iUsername);
        setRecoverySeed(iRecoverySeed);
        setPublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveUsername(iUsername);
        mDataManager->savePublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveRecoverySeed(iRecoverySeed);

        createCryptoWallets();
        emit userCreationFinished(true, "");
    } else {
        qDebug() << "Error creating account...";
        emit userCreationFinished(false, "Failed to create account");
    }
}

void UserAccount::marketValuesUpdated(QStringList iNames, QStringList iValues)
{
    for (int i = 0; i < iNames.size(); i++) {
        qDebug() << iNames.at(i) << " --- " << iValues.at(i);
        mBrightWallets[iNames.at(i)].setMarketValue(iValues.at(i));
    }
    emit marketValueChanged();
}

void UserAccount::walletBalancesUpdated(QString iShortname, QStringList iAddresses, QStringList iBalances, QStringList iPendingBalances)
{
    for (int i = 0; i < iAddresses.size(); i++) {
        mBrightWallets[iShortname].updateBalance(iAddresses.at(i), iBalances.at(i), iPendingBalances.at(i));
    }

    mDataManager->saveWalletBalance(iShortname, getBalance(iShortname), getBalance(iShortname, false));

    emit walletBalanceUpdateComplete(iShortname);
}

void UserAccount::downloaderErrorString(QString iError)
{
    qDebug() << "Error from downloader:  " << iError;
}

void UserAccount::loadAccountFromSettings()
{
    if (mDataManager->userAccountExists()) {
        QString lUsername;
        QString lPub;
        QString lPriv;
        QString lPasscode;
        QString lRecoveryPhrase;

        mDataManager->usernameAndKeys(lUsername, lPub, lPriv);
        lPasscode = mDataManager->getPasscode();
        lRecoveryPhrase = mDataManager->getRecoverySeed();

        setUsername(lUsername);
        setPublicAndPrivateKeys(lPub, lPriv);
        setPasscode(lPasscode);
        setRecoverySeed(lRecoveryPhrase);

        QList<WalletAccount> lWalletAccounts;
        mDataManager->getBrightWalletAccounts(lWalletAccounts);

        qDebug() << "Loading wallets from settings..";
        for (auto lWA : lWalletAccounts) {
            mBrightWallets[lWA.shortName()] = lWA;
            QString lConfBalance, lUnconfBalance;
            mDataManager->getWalletBalance(lWA.shortName(), lConfBalance, lUnconfBalance);
            mBrightWallets[lWA.shortName()].setConfirmedBalance(lConfBalance);
            mBrightWallets[lWA.shortName()].setUnconfirmedBalance(lUnconfBalance);

            qDebug() << "Setting downloader address!";
            emit setDownloaderAddresses(lWA.shortName(), mBrightWallets[lWA.shortName()].getWalletAddresses());
        }

    } else {
        qDebug() << "No account created... On load...";
    }
}

void UserAccount::createCryptoWallets()
{
    for (int i = 0; i < AppWallets::walletNames().keys().size(); i++) {
        QString lShortname = AppWallets::walletNames().keys().at(i);
        QString lLongname = AppWallets::walletNames()[AppWallets::walletNames().keys().at(i)];

        CryptoNetwork lNetwork = CryptoNetwork::Main;
        if (lShortname.at(0) == "t") {
            lNetwork = CryptoNetwork::TestNet;
        }

        WalletAccount lWA(lShortname, lLongname, lNetwork);
        lWA.setDataManager(mDataManager);

        if (!lLongname.contains("Dark", Qt::CaseSensitive) && lShortname.at(0) != "d") {
            lWA.createNewBrightWallet(mRecoverySeed);
            QString lWalletAddress;
            if (lWA.getBrightAddress(lWalletAddress)) {
                emit setDownloaderAddresses(lShortname, QStringList() << lWalletAddress);
            }
            mBrightWallets.insert(lShortname, lWA);
        } else {
            // TODO
            //add dark wallet...
        }
        mDataManager->saveWalletAccount(lShortname, lLongname, lNetwork);
    }
}
