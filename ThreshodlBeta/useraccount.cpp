#include "useraccount.h"

#include <QDebug>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";

    mDataManager = new MyQSettingsManager;
    loadAccountFromSettings();

    mWaiting = false;
    setCurrentErrorString("");

    mMyDownloaderThread = new QThread;
    mMyDownloaderWorker = new DownloadWorker();

    mMyDownloaderWorker->moveToThread(mMyDownloaderThread);

    connect(mMyDownloaderWorker, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
    connect(mMyDownloaderThread, SIGNAL (started()), mMyDownloaderWorker, SLOT (startDownloading()));
    connect(mMyDownloaderWorker, SIGNAL (finished()), mMyDownloaderThread, SLOT (quit()));
    connect(mMyDownloaderWorker, SIGNAL (finished()), mMyDownloaderWorker, SLOT (deleteLater()));
    connect(mMyDownloaderThread, SIGNAL (finished()), mMyDownloaderThread, SLOT (deleteLater()));

    mMyDownloaderThread->start();
}

bool UserAccount::exists()
{
    return mDataManager->userAccountExists();
}

void UserAccount::createNewAccount(QString iUsername)
{
    emit usernameCreated(true, iUsername, QByteArray("TEst"), QByteArray("test"));
    return;

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

void UserAccount::usernameCreated(bool iSuccess, QString iUsername, QByteArray iPublicKey, QByteArray iPrivateKey)
{
    if (iSuccess) {
        setUsername(iUsername);
        setPublicAndPrivateKeys(iPublicKey, iPrivateKey);
        mDataManager->saveUsername(iUsername);
        mDataManager->savePublicAndPrivateKeys(iPublicKey, iPrivateKey);
        setCurrentErrorString("");

        // create wallets for each coin...
        createCryptoWallets();
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

        QList<WalletAccount> lWalletAccounts;
        mDataManager->getWalletAccounts(lWalletAccounts);

        for (auto lWA : lWalletAccounts) {
            mWallets[lWA.shortName()] = lWA;
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
        }

        mWallets.insert(lShortname, lWA);

        mDataManager->saveWalletAccount(lShortname, lLongname, CryptoChain::TestNet);
    }
}
