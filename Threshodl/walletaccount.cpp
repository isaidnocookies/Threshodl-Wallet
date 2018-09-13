#include "walletaccount.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>

WalletAccount::WalletAccount()
{
}

WalletAccount::WalletAccount(QString iShortName, QString iLongName, CryptoNetwork iChain)
{
    mShortName = iShortName;
    mLongName = iLongName;
    mChain = iChain;

    WalletAccount();
}

WalletAccount::WalletAccount(QByteArray iData)
{
    QJsonParseError lError;
    QVariantMap lDataMap = QJsonDocument::fromJson(iData,&lError).toVariant().toMap();

    if (lError.error == QJsonParseError::NoError) {
        mShortName = lDataMap["shortname"].toString();
        mLongName = lDataMap["longname"].toString();
        mIsDark = lDataMap["isDark"].toBool();
        mConfirmedBalance = lDataMap["confirmedBalance"].toString();
        mUnconfirmedBalance = lDataMap["unconfirmedBalance"].toString();
        mChain = static_cast<CryptoNetwork>(lDataMap["chain"].toInt());
        mMarketValue = lDataMap["marketValue"].toString();
        mExchangeCurrency = lDataMap["exchangeCurrency"].toString();

        QList<QVariant> lWallets = lDataMap["wallets"].toList();
        mWallets.clear();

        for (auto sw : lWallets) {
            mWallets.append(CryptoWallet(sw.toByteArray()));
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
    if (mShortName.at(0) == "d") {
        return mShortName;
    }

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

QString WalletAccount::owner()
{
    return mOwner;
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

void WalletAccount::setUnconfirmedBalance(QString iBalance)
{
    mUnconfirmedBalance = iBalance;
}

void WalletAccount::setOwner(QString iOwner)
{
    mOwner = iOwner;
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
    QStringMath lTotalConfirmed;
    QStringMath lTotalUnconfirmed;

    for (int i = 0; i < mWallets.size(); i++) {
        if (mWallets.at(i).address() == iAddress) {
            mWallets[i].setConfirmedBalance(iBalance);
            mWallets[i].setUnconfirmedBalance(iUnconfirmedBalance);
        }
        lTotalConfirmed = lTotalConfirmed + mWallets.at(i).confirmedBalance();
        lTotalUnconfirmed = lTotalUnconfirmed + mWallets.at(i).unconfirmedBalance();
    }

    mConfirmedBalance = lTotalConfirmed.toString();
    mUnconfirmedBalance = lTotalUnconfirmed.toString();
}

void WalletAccount::addWallet(CryptoWallet iWallet)
{
    // check if wallet already exists...
    for (auto w : mWallets) {
        if (w.address() == iWallet.address()) {
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

bool WalletAccount::getWallet(CryptoWallet &oWallet, QString iAddress)
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

bool WalletAccount::hasBrightWallet()
{
    if (mWallets.size() > 0) {
        return true;
    }
    return false;
}

QList<CryptoWallet> WalletAccount::getWallets()
{
    return mWallets;
}

void WalletAccount::createNewBrightWallet(QString iSeed)
{
    CryptoWallet lNewCryptoWallet = CryptoWallet(mShortName, mLongName, iSeed, mChain);
    lNewCryptoWallet.setOwner(mOwner);
    mWallets.append(lNewCryptoWallet);
    mAccountData->saveWallet(lNewCryptoWallet.toData(), mShortName, false);
}

bool WalletAccount::createRawTransaction(QString iToAddress, QString iToAmount, QString &oTxHex, QString &oFee)
{
    QString lCoinName = mShortName;
    auto lCurrentWallet = mWallets[0];
    QString lFromAddress = lCurrentWallet.address();
    QString lFromPrivateKey = lCurrentWallet.privateKey();

    QString lReturnTxHex;
    QString lReturnFee;
    bool lSuccess;

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray toAddresses;
    QJsonArray toAmounts;

    toAddresses.append(iToAddress);
    toAmounts.append(iToAmount);

    jsonData.insert("coin", lCoinName);
    jsonData.insert("fromAddress", lFromAddress);
    jsonData.insert("fromPrivateKey", lFromPrivateKey);
    jsonData.insert("toAddresses", toAddresses);
    jsonData.insert("toAmounts", toAmounts);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QString lRequestData = request_body;
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/createTransaction/"));
    qDebug() << lRequestURL;
    qDebug().noquote() << request_body;

    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = mNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        qDebug() << lReplyText;

        if (lMyMap["success"].toBool()) {
            lReturnTxHex = lMyMap["message"].toMap()["txHex"].toString();
            lReturnFee = lMyMap["message"].toMap()["fee"].toString();
            lSuccess = true;
        } else {
            qDebug() << "Error(1).... Failed to send transaction";
            lReturnTxHex = "";
            lReturnFee = "";
            lSuccess = false;
        }
    } else {
        qDebug() << "Error(2).... Failed to send transaction" << lReply->errorString();
        lReturnTxHex = "";
        lReturnFee = "";
        lSuccess = false;
    }

    oTxHex = lReturnTxHex;
    oFee = lReturnFee;

    return lSuccess;
}

bool WalletAccount::sendRawTransaction(QString iRawTransaction, QString &oTxid)
{
    QString lCoinName = mShortName;
    auto lCurrentWallet = mWallets[0];

    QString lReturnTxid;
    bool lSuccess;

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;

    jsonData.insert("coin", lCoinName);
    jsonData.insert("tx", iRawTransaction);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QString lRequestData = request_body;
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/sendRawTransaction/"));
    qDebug() << lRequestURL;
    qDebug().noquote() << request_body;

    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = mNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        qDebug() << lReplyText;

        if (lMyMap["success"].toBool()) {
            lReturnTxid = lMyMap["txid"].toString();
            lSuccess = true;
        } else {
            qDebug() << "Error(1).... Failed to send transaction";
            lReturnTxid = "";
            lSuccess = false;
        }
    } else {
        qDebug() << "Error(2).... Failed to send transaction" << lReply->errorString();
        lReturnTxid = "";
        lSuccess = false;
    }

    oTxid = lReturnTxid;

    return lSuccess;
}

QString WalletAccount::sendBrightTransaction(QString iToAddress, QString iToAmount)
{
    QString lCoinName = mShortName;
    auto lCurrentWallet = mWallets[0];
    QString lFromAddress = lCurrentWallet.address();
    QString lFromPrivateKey = lCurrentWallet.privateKey();

    QString lReturnTxid;

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray toAddresses;
    QJsonArray toAmounts;

    toAddresses.append(iToAddress);
    toAmounts.append(iToAmount);

    jsonData.insert("coin", lCoinName);
    jsonData.insert("fromAddress", lFromAddress);
    jsonData.insert("fromPrivateKey", lFromPrivateKey);
    jsonData.insert("toAddresses", toAddresses);
    jsonData.insert("toAmounts", toAmounts);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QString lRequestData = request_body;
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/send/"));
    qDebug() << lRequestURL;
    qDebug().noquote() << request_body;

    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = mNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        qDebug() << lReplyText;

        if (lMyMap["success"].toBool()) {
            lReturnTxid = lMyMap["txid"].toString();
        } else {
            qDebug() << "Error(1).... Failed to send transaction";
            lReturnTxid = "";
        }
    } else {
        qDebug() << "Error(2).... Failed to send transaction" << lReply->errorString();
        lReturnTxid = "";
    }

    return lReturnTxid;
}

bool WalletAccount::estimateMicroWallets(QString iAmount, QString &oAmountWithoutFee, int &oBreaks, QString &oFee, QString &oError)
{
    QString lCoinName = mShortName;

    bool lSuccess;
    QString lFee = "";
    int lBreaks = 0;
    QString lAmountWithoutFee = "";

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;

    jsonData.insert("coin", lCoinName);
    jsonData.insert("amount", iAmount);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QString lRequestData = request_body;
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/dark/estimateBreaksAndFees/"));
    qDebug() << lRequestURL;
    qDebug().noquote() << request_body;

    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = mNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        qDebug() << lReplyText;

        if (lMyMap["success"].toBool()) {
            lBreaks = lMyMap["estimate"].toInt();
            lFee = lMyMap["feeEstimate"].toString();
            lSuccess = true;
        } else {
            QString error = "Error(1).... Failed to send transaction";
            oError = error;
            qDebug() << error;
            lSuccess = false;
        }
    } else {
        QString error = "Error(2).... Failed to send transaction" + lReply->errorString();
        qDebug() << error;
        oError = error;
        lSuccess = false;
    }

    lAmountWithoutFee = (QStringMath(iAmount) - lFee).toString();
    oAmountWithoutFee = lAmountWithoutFee;
    oBreaks = lBreaks;
    oFee = lFee;

    return lSuccess;
}

bool WalletAccount::createMicroWallets(QString iAmount, int &oBreaks, QString &oFinalAmount, QString &oError)
{
    QString lCoinName = mShortName;

    if (lCoinName.at(0) == "d") {
        lCoinName.remove(0,1);
    }

    bool lSuccess;
    int breaks;
    QString finalAmount;

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;

    jsonData.insert("coin", lCoinName);
    jsonData.insert("value", iAmount);
    jsonData.insert("owner", mOwner);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QString lRequestData = request_body;
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/dark/createWallets/"));
    qDebug() << lRequestURL;
    qDebug().noquote() << request_body;

    QNetworkRequest lRequest;
    lRequest.setUrl(lRequestURL);
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    lReply = mNetworkManager->post(lRequest, request_body);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        qDebug() << lReplyText;

        if (lMyMap["success"].toBool()) {
            CryptoNetwork lNetwork = (mShortName.contains("t") ? CryptoNetwork::TestNet : CryptoNetwork::Main);
            if (lMyMap["coin"] != mShortName) {
                qDebug() << "Wrong stuffs";
                return false;
            }

            finalAmount = (QStringMath(iAmount) - lMyMap["fee"].toString()).toString();
            auto lCoinWallet = lMyMap["wallets"].toMap();
            breaks = lCoinWallet.keys().size();

            for (auto key : lCoinWallet.keys()) {
                QString lAddress = lCoinWallet[key].toMap()["address"].toString();
                QString lPrivateKey = lCoinWallet[key].toMap()["privateKey"].toString();
                QString lValue = lCoinWallet[key].toMap()["value"].toString();

                CryptoWallet lNewMicroWallet(mShortName, mLongName, "addr", "privateKey", lNetwork);
                lNewMicroWallet.setOwner(mOwner);
                lNewMicroWallet.setValue(lValue);
                lNewMicroWallet.setFilled(false);
                lNewMicroWallet.setIsMicroWallet(true);

                mWallets.append(lNewMicroWallet);
                mAccountData->saveWallet(lNewMicroWallet.toData(), mShortName, true);
            }

            oBreaks = breaks;
            oFinalAmount = finalAmount;
            lSuccess = true;
        } else {
            QString error = "Error(1).... Failed to send transaction";
            oError = error;
            qDebug() << error;
            lSuccess = false;
        }
    } else {
        QString error = "Error(2).... Failed to send transaction" + lReply->errorString();
        qDebug() << error;
        oError = error;
        lSuccess = false;
    }

    return lSuccess;
}

const QByteArray WalletAccount::toData()
{
    QVariantMap lDataMap;
    QList<QVariant> lWallets;

    lDataMap["shortname"] = mShortName;
    lDataMap["longname"] = mLongName;
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
