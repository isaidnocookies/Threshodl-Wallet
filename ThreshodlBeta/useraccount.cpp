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

    mMyDownloaderThread->start();

    loadAccountFromSettings();

//    mDownloaderWaitCondition->wakeAll();
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

void UserAccount::recoverAccount(QString iSeed, QString iUsername)
{
    setRecoverySeed(iSeed);

    //TODO! check seed, create stuffs, then go forward...
    //TODO: recover username from db

    usernameCreated(true, iUsername, iSeed, "", "");
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
    // use marketValues for this..
    if (iCurrency == "USD") {
        return "0.00";
    } else {
        return "0.00";
    }
}

QString UserAccount::getBalance(QString iShortName, bool iIsDark, bool iConfirmed)
{
    Q_UNUSED (iIsDark)
    return mBrightWallets[iShortName].getBalance(iConfirmed);
}

QString UserAccount::getBalanceValue(QString iShortName, bool iIsDark, bool iConfirmed, QString iCurrency)
{
    Q_UNUSED (iIsDark) Q_UNUSED(iCurrency) Q_UNUSED(iConfirmed)

    return mBrightWallets[iShortName].marketValue();
}

bool UserAccount::isWalletConfirmed(QString iCurrency, QString iWalletType)
{
    bool lIsDark;
    if (iWalletType == "Dark") {
        lIsDark = true;
    } else {
        lIsDark = false;
    }

    QString lName = QString(lIsDark ? "d" : "").append(iCurrency);
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

void UserAccount::usernameCreated(bool iSuccess, QString iUsername, QString iRecoverySeed, QByteArray iPublicKey, QByteArray iPrivateKey)
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

    //emit crypto specific signal
    emit walletBalanceUpdateComplete(iShortname);
//    emit cryptoBalanceUpdated();
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

        mDataManager->usernameAndKeys(lUsername, lPub, lPriv);
        lPasscode = mDataManager->getPasscode();

        setUsername(lUsername);
        setPublicAndPrivateKeys(lPub, lPriv);
        setPasscode(lPasscode);

        QList<WalletAccount> lWalletAccounts;
        mDataManager->getBrightWalletAccounts(lWalletAccounts);

        qDebug() << "Loading wallets from settings..";
        for (auto lWA : lWalletAccounts) {
            mBrightWallets[lWA.shortName()] = lWA;
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
