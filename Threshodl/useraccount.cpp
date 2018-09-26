#include "useraccount.h"
#include "darkwallettools.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication>
#include <QObject>
#include <QDesktopServices>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";
    mDataManager = new MyQSettingsManager;

    QDesktopServices::setUrlHandler("file", this, "handleFileUrlReceived");

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
    if (iShortName.at(0) != "d") {
        return mBrightWallets[iShortName].getBalance(iConfirmed);
    } else {
        return mDarkWallets[iShortName].getBalance(false);
    }
}

QString UserAccount::getBalanceValue(QString iShortName, bool iConfirmed, QString iCurrency)
{
    Q_UNUSED(iCurrency) Q_UNUSED(iConfirmed)
    bool isConfirmed = (iShortName.contains("d") ? false : true);

    QString lCoinTotal;

    double lMarketValue = getMarketValue(iShortName).toDouble();
    double lBalance = getBalance(iShortName, isConfirmed).toDouble();

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

    QString lName = iShortname;
    if (!lIsDark) {
        if (QStringMath(mBrightWallets[lName].getBalance()) == QStringMath(mBrightWallets[lName].getBalance(false))) {
            return true;
        }
    } else {
        if (QStringMath(mDarkWallets[lName].getBalance()) == QStringMath(mDarkWallets[lName].getBalance(false))) {
            return true;
        }
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
    QString lName = iShortname;

    if (lName.at(0) == "d") {
        lName.remove(0,1);
    }

    return mBrightWallets[lName].marketValue();
}

QString UserAccount::getEmailAddress()
{
    return emailAddress();
}

void UserAccount::setEmailAddress(QString iEmail)
{
    if (mEmailAddress == iEmail) {
        return;
    }

    mEmailAddress = iEmail;
    mDataManager->saveEmailAddress(iEmail);
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
    lWA.setOwner(mUsername);
    lWA.setPublicKey(mPublicKey);
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

void UserAccount::startDarkDeposit(QString iShortname, QString iAmount)
{
    if (!mDarkWallets.contains(iShortname)) {
        CryptoNetwork network;
        QString lLongname = AppWallets::walletNames()[iShortname];

        if (iShortname.contains("t")) {
            network = CryptoNetwork::TestNet;
        } else {
            network = CryptoNetwork::Main;
        }

        mDarkWallets.insert(iShortname, WalletAccount(iShortname, lLongname, network));
        mDarkWallets[iShortname].setOwner(mUsername);
        mDarkWallets[iShortname].setPublicKey(mPublicKey);

        mDataManager->saveWalletAccount(iShortname, lLongname, network);
    }

    mDarkWallets[iShortname].setDataManager(mDataManager);

    QString oAmountWithoutFee;
    int oBreaks;
    QString oFee;
    QString oError;

    mDarkWallets[iShortname].estimateMicroWallets(iAmount,oAmountWithoutFee,oBreaks,oFee,oError);
    emit darkDepositConfirmation(true, oFee, oAmountWithoutFee, iShortname);
}

void UserAccount::depositDarkCoin(QString iShortname, QString iAmount)
{
    if (!mDarkWallets.contains(iShortname)) {
        CryptoNetwork network;
        QString lLongname = AppWallets::walletNames()[iShortname];

        if (iShortname.contains("t")) {
            network = CryptoNetwork::TestNet;
        } else {
            network = CryptoNetwork::Main;
        }

        mDarkWallets.insert(iShortname, WalletAccount(iShortname, lLongname, network));
        mDarkWallets[iShortname].setOwner(mUsername);
        mDarkWallets[iShortname].setPublicKey(mPublicKey);

        mDataManager->saveWalletAccount(iShortname, lLongname, network);
    }

    mDarkWallets[iShortname].setDataManager(mDataManager);

    QString lError;
    QString lFinalAmount;
    int lBreaks;
    if (mDarkWallets[iShortname].createMicroWallets(iAmount, lBreaks, lFinalAmount, mPublicKey, lError)) {
        emit darkDepositComplete(true, lFinalAmount, lBreaks);
    } else {
        emit darkDepositComplete(false, "", 0);
    }
}

void UserAccount::estimateDarkTransaction(QString iShortname, QString toAmount)
{
    qDebug() << "Estimating dark transaction fees for ... " + iShortname;
    DarkWalletTools lDarkTools;
    lDarkTools.setUserDetails(mUsername, mPublicKey, mPrivateKey);
    QString fee = lDarkTools.estimateFeesForDark(toAmount, iShortname);
    bool success = false;

    if (fee == "-1") {
        success = false;
    } else {
        success = true;
    }

    emit darkTransactionEstimated(success, fee);
}

void UserAccount::checkIfDarkTransactionIsPossible(QString iShortname, QString sendAmount)
{
    ErrorCodes::DarkErrors lError;
    bool lSuccess = false;
    QStringMath lSumOfValidWallets;

    qDebug() << "Checking if " + iShortname + " transaction is possible...";
    if (mDarkWallets.contains(iShortname)) {
        mDarkWallets[iShortname].sortDarkWallets();
        QList<CryptoWallet> lWallets = mDarkWallets[iShortname].getWallets();
        for (auto lW : lWallets) {
            if ((lSumOfValidWallets + lW.value()) <= sendAmount) {
                lSumOfValidWallets = lSumOfValidWallets + lW.value();
            }
        }

        if (lSumOfValidWallets == QStringMath(sendAmount)) {
            lError = ErrorCodes::DarkErrors::None;
            lSuccess = true;
        } else {
            lError = ErrorCodes::DarkErrors::NotEnoughChange;
            lSuccess = false;
        }
    } else {
        lError = ErrorCodes::DarkErrors::NoMicroWallets;
    }

    emit darkTransactionIsPossible(lSuccess, lError);
}

void UserAccount::sendDarkTransaction(QString iAmount, QString iShortname, QString toAddress, QString toEmail)
{
    DarkWalletTools lDarkTools;
    ErrorCodes::DarkErrors lError;
    bool lSuccess = false;
    QStringMath lTotal;
    QList<CryptoWallet> lWalletsToSave;
    QList<CryptoWallet> lWalletsToSend;
    QVariantList lWallets;

    lDarkTools.setUserDetails(mUsername, mPublicKey, mPrivateKey);

    mDarkWallets[iShortname].sortDarkWallets();
    auto darkWallets = mDarkWallets[iShortname].getWallets();

    for (auto ldw : darkWallets) {
        if (QStringMath((lTotal + ldw.value()).toString()) <= iAmount) {
            lWalletsToSend.append(ldw);
            lWallets.append(ldw.toData());
            lTotal = lTotal + ldw.value();
        } else {
            lWalletsToSave.append(ldw);
        }
    }

    if (lDarkTools.sendWallets(lWallets, username(), iShortname, toEmail, toAddress, lError)) {
        lSuccess = true;
        qDebug() << "Dark transaction successful";
    } else {
        qDebug() << "Dark transaction failed";
        lSuccess = false;
        lWalletsToSave.append(lWalletsToSend);
    }
    mDarkWallets[iShortname].setDarkWallets(lWalletsToSave);

    emit darkTransactionSent(lSuccess, lError);
}

QVariantList UserAccount::getDarkWallets(QString iShortname)
{
    QVariantList lList;

    if (!mDarkWallets.contains(iShortname)) {
        CryptoNetwork network;
        QString lLongname = AppWallets::walletNames()[iShortname];

        if (iShortname.contains("t")) {
            network = CryptoNetwork::TestNet;
        } else {
            network = CryptoNetwork::Main;
        }

        mDarkWallets.insert(iShortname, WalletAccount(iShortname, lLongname, network));
        mDarkWallets[iShortname].setPublicKey(mPublicKey);
        mDarkWallets[iShortname].setOwner(mUsername);

        mDataManager->saveWalletAccount(iShortname, lLongname, network);
    }
    mDarkWallets[iShortname].setDataManager(mDataManager);


    mDarkWallets[iShortname].sortDarkWallets();
    WalletAccount lDarkWallets = mDarkWallets[iShortname];
    auto lWallets = lDarkWallets.getWallets();

    if (lWallets.size() > 0) {
        for (auto wallet : lWallets) {
            QVariantList lWalletList;
            lWalletList.append(wallet.value());
            lWalletList.append(wallet.address());
            lWalletList.append(wallet.privateKey());
            lList.append(lWalletList);
        }
    }
    return lList;
}

void UserAccount::handleFileUrlReceived(const QUrl &url)
{
    QString incomingUrl = url.toString();
    if(incomingUrl.isEmpty()) {
        qWarning() << "setFileUrlReceived: we got an empty URL";
        return;
    }
    QString myUrl;
    if(incomingUrl.startsWith("file://")) {
        myUrl= incomingUrl.right(incomingUrl.length()-7);
        qDebug() << "QFile needs this URL: " << myUrl;
    } else {
        myUrl= incomingUrl;
    }

    QFileInfo fileInfo = QFileInfo(myUrl);
    if(fileInfo.exists()) {
        qDebug() << "Received! YA BITCHES!!";

        QFile lPackage(myUrl);
        QByteArray lPackageArray;

        if (!lPackage.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open package";
        } else {
            lPackageArray = lPackage.readAll();
            QJsonDocument lDoc = QJsonDocument::fromJson(lPackageArray);
            QJsonObject lObject = lDoc.object();

            qDebug() << lPackageArray;

            if (lObject["Action"].toString() == "Dark Transaction") {
                QJsonArray lArray = lObject["wallets"].toArray();

                QString amount = lObject["Amount"].toString();
                QString notes = lObject["Notes"].toString();
                QString type = lObject["Type"].toString();
                QJsonArray lWallets = lObject["Wallets"].toArray();
                QVariantList lWalletsToImport;

                for (auto lw : lWallets) {
                    lWalletsToImport.append(lw.toVariant().toByteArray());
                }

                qDebug() << "Emitting signal...";
                emit importDarkWalletsSignal(type, amount, notes, lWalletsToImport);
            }
        }

    } else {
        qDebug() << "setFileUrlReceived: FILE does NOT exist ";
    }
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
        emit marketValueChanged(iNames.at(i));
    }
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
        QString lEmail;
        QString lPub;
        QString lPriv;
        QString lPasscode;
        QString lRecoveryPhrase;

        mDataManager->usernameAndKeys(lUsername, lPub, lPriv);
        lPasscode = mDataManager->getPasscode();
        lRecoveryPhrase = mDataManager->getRecoverySeed();
        lEmail = mDataManager->getEmail();

        setUsername(lUsername);
        setPublicAndPrivateKeys(lPub, lPriv);
        setPasscode(lPasscode);
        setRecoverySeed(lRecoveryPhrase);
        setEmailAddress(lEmail);

        QList<WalletAccount> lWalletAccounts;
        QList<WalletAccount> lDarkWalletAccounts;

        qDebug() << "Loading wallets from settings..";
        mDataManager->getBrightWalletAccounts(lWalletAccounts);
        for (auto lWA : lWalletAccounts) {
            mBrightWallets[lWA.shortName()] = lWA;
            QString lConfBalance, lUnconfBalance;
            mDataManager->getWalletBalance(lWA.shortName(), lConfBalance, lUnconfBalance);
            mBrightWallets[lWA.shortName()].setConfirmedBalance(lConfBalance);
            mBrightWallets[lWA.shortName()].setUnconfirmedBalance(lUnconfBalance);
            mBrightWallets[lWA.shortName()].setDataManager(mDataManager);

            qDebug() << "Setting downloader address!";
            emit setDownloaderAddresses(lWA.shortName(), mBrightWallets[lWA.shortName()].getWalletAddresses());
        }

        qDebug() << "Loading DARK wallets from settings..";
        mDataManager->getDarkWalletAccounts(lDarkWalletAccounts);
        for (auto lDWA : lDarkWalletAccounts) {
            mDarkWallets[lDWA.shortName()] = lDWA;

            auto mDarkMicros = lDWA.getWallets();
            QStringMath lConfirmed;
            QStringMath lUnconfirmed;
            for (auto mw : mDarkMicros) {
                lConfirmed = lConfirmed + mw.confirmedBalance();
                lUnconfirmed = lUnconfirmed + mw.value();
            }

//            QString lConfBalance, lUnconfBalance;
//            mDataManager->getDarkWalletBalance(lDWA.shortName(), lConfBalance, lUnconfBalance);
//            mDarkWallets[lDWA.shortName()].setConfirmedBalance(lConfBalance);
//            mDarkWallets[lDWA.shortName()].setUnconfirmedBalance(lUnconfBalance);

            mDarkWallets[lDWA.shortName()].setConfirmedBalance(lConfirmed.toString());
            mDarkWallets[lDWA.shortName()].setUnconfirmedBalance(lUnconfirmed.toString());

            mDarkWallets[lDWA.shortName()].setDataManager(mDataManager);
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
            mDataManager->saveWalletAccount(lShortname, lLongname, lNetwork);
        } else {
            mDarkWallets.insert(lShortname, lWA);
            mDataManager->saveWalletAccount(lShortname, lLongname, lNetwork);
        }
    }
}
