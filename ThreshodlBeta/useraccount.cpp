#include "useraccount.h"

#include <QDebug>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";

    mDataManager = new MyQSettingsManager;

    mWaiting = false;
    setCurrentErrorString("");

    mMyDownloaderThread = new QThread;
    mMyDownloaderWorker = new DownloadWorker();

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
//    emit usernameCreated(true, iUsername, QByteArray("TEst"), QByteArray("test"));
//    return;

    setCurrentErrorString("");
    setWaiting(true);

    mCreateUsername = new CreateUsername;
    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    mCreateUsername->create(iUsername);
}

QString UserAccount::getRecoverySeed()
{
    return mRecoverySeed;
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

    return mWallets[iShortName].getBalance(iConfirmed);
}

QString UserAccount::getBalanceValue(QString iShortName, bool iIsDark, bool iConfirmed, QString iCurrency)
{
    Q_UNUSED (iIsDark) Q_UNUSED(iCurrency) Q_UNUSED(iConfirmed)

    return mWallets[iShortName].marketValue();
}

bool UserAccount::isWalletConfirmed(QString iCurrency, QString iWalletType)
{
    return true;
    bool lIsDark;
    if (iWalletType == "Dark") {
        lIsDark = true;
    } else {
        lIsDark = false;
    }

    QString lName = QString(lIsDark ? "d" : "").append(iCurrency);

    if (QStringMath(mWallets[lName].getBalance(false)) == "0.0") {
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

void UserAccount::setWaiting(bool iWaiting)
{
    if (iWaiting == mWaiting) { return; }

    mWaiting = iWaiting;
    emit waitingChanged();
}

bool UserAccount::waiting()
{
    return mWaiting;
}

QString UserAccount::getBrightAddress(QString iShortname)
{
    auto lAccount = mWallets[iShortname];
    QString lAddress;
    if (lAccount.getBrightAddress(lAddress)) {
        return lAddress;
    }
    return "";
}

QString UserAccount::getMarketValue(QString iShortname, QString iCurrency)
{
    Q_UNUSED(iCurrency)

    auto lAccount = mWallets[iShortname];
    QString lMarketValue = lAccount.marketValue();

    return lMarketValue;
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

void UserAccount::usernameCreated(bool iSuccess, QString iUsername, QString iRecoverySeed, QByteArray iPublicKey, QByteArray iPrivateKey)
{
    if (iSuccess) {
        setUsername(iUsername);
        setRecoverySeed(iRecoverySeed);
        setPublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveUsername(iUsername);
        mDataManager->savePublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveRecoverySeed(iRecoverySeed);
        setCurrentErrorString("");

        // create wallets for each coin...
        createCryptoWallets();
    } else {
        setCurrentErrorString("Failed to create account");
        qDebug() << "Error creating account...";
    }
    setWaiting(false);
}

void UserAccount::marketValuesUpdated(QStringList iNames, QStringList iValues)
{
    for (int i = 0; i < iNames.size(); i++) {
        qDebug() << iNames.at(i) << " --- " << iValues.at(i);
        mWallets[iNames.at(i)].setMarketValue(iValues.at(i));
    }
    emit marketValueChanged();
}

void UserAccount::walletBalancesUpdated(QString iShortname, QStringList iAddresses, QStringList iBalances, QStringList iPendingBalances)
{
    for (int i = 0; i < iAddresses.size(); i++) {
        mWallets[iShortname].updateBalance(iAddresses.at(i), iBalances.at(i), iPendingBalances.at(i));
    }

    emit cryptoBalanceUpdated();
}

void UserAccount::downloaderErrorString(QString iError)
{
    qDebug() << "Error from downloader:  " << iError;
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

        QList<WalletAccount> lWalletAccounts;
        mDataManager->getWalletAccounts(lWalletAccounts);

        qDebug() << "Loading wallets from settings..";
        for (auto lWA : lWalletAccounts) {
            mWallets[lWA.shortName()] = lWA;
            qDebug() << "Setting downloader address";
            emit setDownloaderAddresses(lWA.shortName(), mWallets[lWA.shortName()].getWalletAddresses());
        }

    } else {
        qDebug() << "No account created... On load...";
    }
}

void UserAccount::createCryptoWallets()
{
    for (int i = 0; i < AppWallets::walletShortnames().size(); i++) {
        QString lShortname = AppWallets::walletShortnames().at(i);
        QString lLongname = AppWallets::walletLongnames().at(i);

        WalletAccount lWA(lShortname, lLongname, CryptoChain::TestNet);
        lWA.setDataManager(mDataManager);

        if (!lLongname.contains("Dark", Qt::CaseInsensitive)) {
            lWA.createNewBrightWallet();
            QString lWalletAddress;
            if (lWA.getBrightAddress(lWalletAddress)) {
                qDebug() << "Setting downloader addresses";
                emit setDownloaderAddresses(lShortname, QStringList() << lWalletAddress);
            }
        }

        mWallets.insert(lShortname, lWA);

        mDataManager->saveWalletAccount(lShortname, lLongname, CryptoChain::TestNet);
    }
}
