#include "genericwallet.h"

#include <QJsonArray>
#include <QJsonObject>

GenericWallet::GenericWallet() : Wallet()
{ }

GenericWallet::GenericWallet(const QByteArray iData) : Wallet(iData)
{ }

GenericWallet::GenericWallet(const Wallet &iOther) : Wallet(iOther)
{ }

GenericWallet::GenericWallet(const WalletDataCore &iOther) : Wallet(iOther)
{ }

GenericWallet GenericWallet::createWallet(QString iShortname, QString iSeed, GenericWallet::ChainType iChainType)
{
    GenericWallet::ChainType lNetwork = iChainType;

    if (iShortname.at(0) == "t") {
        lNetwork = GenericWallet::ChainType::TestNet;
    } else {
        lNetwork = GenericWallet::ChainType::Main;
    }

    if (iShortname == "BTC") {
        return createBitcoinWallet(iSeed, lNetwork);
    } else if (iShortname == "tBTC") {
        return createBitcoinWallet(iSeed, lNetwork);
    }else if (iShortname == "ETH") {
        return createEthWallet(iSeed, lNetwork);
    } else if (iShortname == "LTC") {
        return createLitecoinWallet(iSeed, lNetwork);
    } else if (iShortname == "DASH") {
        return createDashWallet(iSeed, lNetwork);
    } else if (iShortname == "ZEC") {
        return createZCashWallet(iSeed, lNetwork);
    }

    return GenericWallet{};
}

GenericWallet GenericWallet::createDashWallet(QString iSeed, GenericWallet::ChainType iChainType)
{
    Wallet lNewWallet;
    QString lCoin = "DASH";
    QString lCoinLong = "Dash";
    int lNetworkType;

    if (iChainType == GenericWallet::ChainType::TestNet) {
        lNetworkType = 2;
    } else {
        // Mainnet
        lNetworkType = 1;
    }

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray coinArray;

    coinArray.append(lCoin);

    jsonData.insert("coins", coinArray);
    jsonData.insert("seed", iSeed);
    jsonData.insert("network", lNetworkType);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/create/"));
    qDebug() << lRequestURL;

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
            auto lCoinMap = lMyMap["message"].toMap();
            auto lCoinWallet = lCoinMap[lCoin].toMap();

            lNewWallet.setAddress(lCoinWallet["address"].toByteArray());
            lNewWallet.setIsMicroWallet(false);
            lNewWallet.setPrivateKey(lCoinWallet["wif"].toByteArray());
            lNewWallet.setShortNameType(lCoin);
            lNewWallet.setLongNameType(lCoinLong);
        } else {
            qDebug() << "Error(1).... Failed to create wallet " << lCoin;
        }
    } else {
        qDebug() << "Error(2).... Failed to create wallet : " << lCoin;
    }

    return lNewWallet;
}

GenericWallet GenericWallet::createLitecoinWallet(QString iSeed, GenericWallet::ChainType iChainType)
{
    Wallet lNewWallet;
    QString lCoin = "LTC";
    QString lCoinLong = "Litecoin";
    int lNetworkType;

    if (iChainType == GenericWallet::ChainType::TestNet) {
        lNetworkType = 2;
    } else {
        // Mainnet
        lNetworkType = 1;
    }

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray coinArray;

    coinArray.append(lCoin);

    jsonData.insert("coins", coinArray);
    jsonData.insert("seed", iSeed);
    jsonData.insert("network", lNetworkType);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/create/"));
    qDebug() << lRequestURL;

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
            auto lCoinMap = lMyMap["message"].toMap();
            auto lCoinWallet = lCoinMap[lCoin].toMap();

            lNewWallet.setAddress(lCoinWallet["address"].toByteArray());
            lNewWallet.setIsMicroWallet(false);
            lNewWallet.setPrivateKey(lCoinWallet["wif"].toByteArray());
            lNewWallet.setShortNameType(lCoin);
            lNewWallet.setLongNameType(lCoinLong);
        } else {
            qDebug() << "Error(1).... Failed to create wallet " << lCoin;
        }
    } else {
        qDebug() << "Error(2).... Failed to create wallet : " << lCoin;
    }

    return lNewWallet;
}

GenericWallet GenericWallet::createZCashWallet(QString iSeed, GenericWallet::ChainType iChainType)
{
    Wallet lNewWallet;
    QString lCoin = "ZEC";
    QString lCoinLong = "ZCash";
    int lNetworkType;

    if (iChainType == GenericWallet::ChainType::TestNet) {
        lNetworkType = 2;
    } else {
        // Mainnet
        lNetworkType = 1;
    }

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray coinArray;

    coinArray.append(lCoin);

    jsonData.insert("coins", coinArray);
    jsonData.insert("seed", iSeed);
    jsonData.insert("network", lNetworkType);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/create/"));
    qDebug() << lRequestURL;

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
            auto lCoinMap = lMyMap["message"].toMap();
            auto lCoinWallet = lCoinMap[lCoin].toMap();

            lNewWallet.setAddress(lCoinWallet["address"].toByteArray());
            lNewWallet.setIsMicroWallet(false);
            lNewWallet.setPrivateKey(lCoinWallet["wif"].toByteArray());
            lNewWallet.setShortNameType(lCoin);
            lNewWallet.setLongNameType(lCoinLong);
        } else {
            qDebug() << "Error(1).... Failed to create wallet " << lCoin;
        }
    } else {
        qDebug() << "Error(2).... Failed to create wallet : " << lCoin;
    }

    return lNewWallet;
}

GenericWallet GenericWallet::createEthWallet(QString iSeed, GenericWallet::ChainType iChainType)
{
    return Wallet();
}

GenericWallet GenericWallet::createBitcoinWallet(QString iSeed, GenericWallet::ChainType iChainType)
{
    Wallet lNewWallet;
    QString lCoin = "BTC";
    QString lCoinLong = "Bitcoin";
    int lNetworkType;

    if (iChainType == GenericWallet::ChainType::TestNet) {
        lNetworkType = 2;
    } else {
        // Mainnet
        lNetworkType = 1;
    }

    QNetworkAccessManager   *mNetworkManager = new QNetworkAccessManager();
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QObject::connect(mNetworkManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    QJsonObject jsonData;
    QJsonArray coinArray;

    coinArray.append(lCoin);

    jsonData.insert("coins", coinArray);
    jsonData.insert("seed", iSeed);
    jsonData.insert("network", lNetworkType);

    QJsonDocument jsonDataDocument;
    jsonDataDocument.setObject(jsonData);

    QByteArray request_body = jsonDataDocument.toJson();
    QUrl lRequestURL = QUrl::fromUserInput(QString(MY_WALLET_SERVER_ADDRESS).append("/wallets/create/"));
    qDebug() << lRequestURL;

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
            auto lCoinMap = lMyMap["message"].toMap();
            auto lCoinWallet = lCoinMap[lCoin].toMap();

            if (iChainType == GenericWallet::ChainType::TestNet) {
                lNewWallet.setShortNameType("tBTC");
                lNewWallet.setLongNameType("Testnet Bitcoin");
            } else {
                lNewWallet.setShortNameType(lCoin);
                lNewWallet.setLongNameType(lCoinLong);
            }

            lNewWallet.setAddress(lCoinWallet["address"].toByteArray());
            lNewWallet.setIsMicroWallet(false);
            lNewWallet.setPrivateKey(lCoinWallet["wif"].toByteArray());
        } else {
            qDebug() << "Error(1).... Failed to create wallet " << lCoin;
        }
    } else {
        qDebug() << "Error(2).... Failed to create wallet : " << lCoin;
    }

    return lNewWallet;
}
