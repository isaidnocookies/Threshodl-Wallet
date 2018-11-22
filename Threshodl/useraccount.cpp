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
#include <QCryptographicHash>

UserAccount::UserAccount(QObject *parent) : QObject(parent)
{
    qDebug() << "UserAccount created";
    mDataManager = new MyQSettingsManager;
    mCreateUsername = nullptr;

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

#if !TURN_OFF_POLLING
    mMyDownloaderThread->start();
#endif

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
    if (mCreateUsername == nullptr) {
        mCreateUsername = new CreateUsername;
    }

    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    mCreateUsername->create(iUsername);
}

void UserAccount::recoverAccount(QString iSeed)
{
    setRecoverySeed(iSeed);

    if (mCreateUsername == nullptr) {
        mCreateUsername = new CreateUsername;
    }

    connect (mCreateUsername, &CreateUsername::usernameCreated, this, &UserAccount::usernameCreated);
    mCreateUsername->recoverAccount(iSeed);
}

QString UserAccount::getRecoverySeed()
{
    return mRecoverySeed;
}

void UserAccount::changeUsername(QString iUsername)
{
    bool lSuccess = false;

    if (iUsername.length() < 0) {
        emit usernameChangeSuccess(false);
        return;
    }

    if (mCreateUsername == nullptr) {
        mCreateUsername = new CreateUsername;
    }
    mCreateUsername->setUserData(mPrivateKey, mPublicKey, mUsername);

    QString newUsername = mCreateUsername->changeUsername(iUsername);
    if (newUsername == "") {
        lSuccess = false;
    } else {
        lSuccess = true;
        setUsername(newUsername);
    }

    emit usernameChangeSuccess(lSuccess);
}

bool UserAccount::checkPasscode(QString iPass)
{
    if (this->hashValue(iPass) == mPasscode) {
        return true;
    }
    return false;
}

void UserAccount::changePasscode(QString iNewPass)
{
    mTempPasscode = this->hashValue(iNewPass);
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
    mDataManager->saveUsername(iUsername);
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
    bool lSuccess = mBrightWallets[iShortname].createBrightRawTransaction(toAddress, toAmount, lTxHex, lFee);
    emit rawTransactionCreated(lSuccess, lTxHex, lFee);
}

void UserAccount::sendRawTransaction(QString iShortname, QString iRawTransaction)
{
    QString lTxid;
    bool lSuccess = mBrightWallets[iShortname].sendRawTransaction(iRawTransaction, lTxid);
    emit rawTransactionSent(lSuccess, lTxid);
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

void UserAccount::withdrawDarkCoin(QString iShortname, QString iAmount)
{
    DarkWalletTools lDarkTools;
    ErrorCodes::DarkErrors lError = ErrorCodes::DarkErrors::None;
    bool lSuccess = true;
    QStringMath lTotal;
    QList<CryptoWallet> lWalletsToSave;
    QList<CryptoWallet> lWalletsToSend;
    QString lBrightShortname;

    if (iShortname.contains("d")) {
        lBrightShortname = iShortname.remove("d");
    }

    lDarkTools.setUserDetails(mUsername, mPublicKey, mPrivateKey);

    mDarkWallets[iShortname].sortDarkWallets();
    auto darkWallets = mDarkWallets[iShortname].getWallets();

    for (auto ldw : darkWallets) {
        if (QStringMath((lTotal + ldw.value()).toString()) <= iAmount) {
            lWalletsToSend.append(ldw);
            lTotal = lTotal + ldw.value();
        } else {
            lWalletsToSave.append(ldw);
        }
    }

    // CHANGE WITH THE ADD BLOCKCHAIN SUPPORT STUFF
    for (auto lw : lWalletsToSend) {
        mBrightWallets[lBrightShortname].addWallet(lw);
        mBrightWallets[lBrightShortname].setConfirmedBalance((QStringMath(mBrightWallets[lBrightShortname].getBalance(true)) + lw.value()).toString());
        mBrightWallets[lBrightShortname].setConfirmedBalance((QStringMath(mBrightWallets[lBrightShortname].getBalance(false)) + lw.value()).toString());
        mDarkWallets[iShortname].setConfirmedBalance((QStringMath(mDarkWallets[iShortname].getBalance(true)) - lw.value()).toString());
        //mDarkWallets[iShortname].setConfirmedBalance((QStringMath(mDarkWallets[iShortname].getBalance(false)) - lw.value()).toString());
    }

    emit walletBalanceUpdateComplete(lBrightShortname);
    emit darkCryptoConfirmedBalanceChanged();
    emit darkCryptoUnconfirmedBalanceChanged();
    emit darkWithdrawComplete(lSuccess, lError);
}

void UserAccount::estimateDarkWithdrawal(QString iShortname, QString iAmount)
{
    DarkWalletTools lDarkTool;
    QStringMath lTotal;
    int walletCounter = 0;
    QString fee;
    bool success = true;

    lDarkTool.setUserDetails(mUsername, mPublicKey, mPrivateKey);

    auto wallets = mDarkWallets[iShortname].getWallets();
    for (auto w : wallets) {
        if (QStringMath(lTotal + w.value()) > iAmount) {
            lTotal = lTotal + w.value();
            walletCounter++;
            break;
        } else {
            lTotal = lTotal + w.value();
            walletCounter++;
        }
    }

    if (lTotal < iAmount) {
        success = false;
    } else {
        fee = lDarkTool.estimateFeesForWithdrawal(walletCounter, iShortname);
        if (fee == "-1") {
            success = false;
        }
    }

    emit darkWithdrawalEstimated(success, fee);
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

            if (lObject["action"].toString() == "Dark Transaction") {
                QJsonArray lArray = lObject["wallets"].toArray();

                QString amount = lObject["amount"].toString();
                QString notes = lObject["notes"].toString();
                QString type = lObject["type"].toString();
                QJsonArray lWallets = lObject["wallets"].toArray();
                QVariantList lWalletsToImport;

                for (auto lw : lWallets) {
                    lWalletsToImport.append(lw.toVariant().toByteArray());
                }

                qDebug() << "Emitting signal...";
                mWalletDataToImport = lPackageArray;
                emit importDarkWalletsSignal(type, amount, notes);
            }
        }

    } else {
        qDebug() << "setFileUrlReceived: FILE does NOT exist ";
    }
}

bool UserAccount::importWallets()
{
    QJsonDocument lDoc = QJsonDocument::fromJson(mWalletDataToImport);
    QJsonObject lObject = lDoc.object();

    QString amount = lObject["amount"].toString();
    QString notes = lObject["notes"].toString();
    QString type = lObject["type"].toString();
    QJsonArray lWallets = lObject["wallets"].toArray();
    QList<CryptoWallet> lWalletsToImport;

    for (auto lw : lWallets) {
        CryptoWallet newWallet(lw.toVariant().toByteArray());
        lWalletsToImport.append(newWallet);
    }

    if (amount.isEmpty() || type.isEmpty() || lWalletsToImport.isEmpty()) {
        return false;
    }

    if (!mDarkWallets.contains(type)) {
        CryptoNetwork network;
        QString lLongname = AppWallets::walletNames()[type];

        if (type.contains("t")) {
            network = CryptoNetwork::TestNet;
        } else {
            network = CryptoNetwork::Main;
        }

        mDarkWallets.insert(type, WalletAccount(type, lLongname, network));
        mDarkWallets[type].setPublicKey(mPublicKey);
        mDarkWallets[type].setOwner(mUsername);

        mDataManager->saveWalletAccount(type, lLongname, network);
    }

    for (auto lw : lWalletsToImport) {
        mDarkWallets[type].addWallet(lw);
    }

    emit darkCryptoConfirmedBalanceChanged();
    emit darkCryptoUnconfirmedBalanceChanged();

    return true;
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

QString UserAccount::hashValue(QString iValue)
{
    QString returnHash;

    QCryptographicHash lHasher (QCryptographicHash::Md5);
    lHasher.addData(iValue.toUtf8());
    returnHash = QString(lHasher.result().toHex());

    return returnHash;
}
