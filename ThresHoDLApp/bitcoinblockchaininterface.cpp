#include "bitcoinblockchaininterface.h"

extern "C" {
    #include <btc/chainparams.h>
    #include <btc/ecc.h>
    #include <btc/tool.h>
    #include <btc/utils.h>

    #include <btc/tx.h>
    #include <btc/cstr.h>
    #include <btc/ecc_key.h>
    #include <btc/script.h>
    #include <btc/utils.h>

    #include <btc/tool.h>
    #include <btc/bip32.h>

    #include <btc/base58.h>
}

#include <QEventLoop>
#include <QUrl>

BitcoinBlockchainInterface::BitcoinBlockchainInterface(QObject *parent) : QObject(parent)
{
    mNetworkAccessManager = new QNetworkAccessManager(this);
}

void BitcoinBlockchainInterface::setActiveUser(UserAccount *iActiveUser)
{
    mActiveUser = iActiveUser;
}

bool BitcoinBlockchainInterface::updateBrightWalletBalances(int iConfirmations)
{
    Q_UNUSED (iConfirmations)

    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QStringMath             lBrightBalance = QStringMath();
    QList<BitcoinWallet>    lNewWallets;
    bool                    lSuccess = true;

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (BitcoinWallet &w : mActiveUser->getBrightWallets()) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addr/%2/balance").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(QString(w.address())))));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            BitcoinWallet   lNewWallet = w;
            QStringMath     lBalance = QStringMath::btcFromSatoshi(QString(lReplyText));
            lNewWallet.setValue(lBalance.toString());
            lNewWallets.append(lNewWallet);
            lBrightBalance = lBrightBalance + lBalance;
        } else {
            qDebug() << "Error.... Can not update bright wallet balance...";
            lSuccess = false;
            lNewWallets.append(w);
        }
    }

    mActiveUser->setBrightWallets(lNewWallets);
    return lSuccess;
}

bool BitcoinBlockchainInterface::updateDarkWalletBalances(int iConfirmations)
{
    Q_UNUSED (iConfirmations)

    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QStringMath             lDarkBalance = QStringMath();
    QStringMath             lPendingDarkBalance = QStringMath();
    QList<BitcoinWallet>    lNewWallets;
    bool                    lSuccess = true;

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    for (BitcoinWallet &w : mActiveUser->getDarkWallets()) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addr/%2/balance").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(QString(w.address())))));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            QStringMath     lBalance = QStringMath::btcFromSatoshi(QString(lReplyText));
            lDarkBalance = lDarkBalance + lBalance;
            lPendingDarkBalance = lPendingDarkBalance + w.value();
        } else {
            qDebug() << "Error.... Can not update dark wallet balance...";
            lSuccess = false;
        }
    }

    if (lPendingDarkBalance == lDarkBalance) {
        mActiveUser->setDarkWalletsSettled(true);
    } else {
        mActiveUser->setDarkWalletsSettled(false);
    }

    qDebug() << "PendingDarkBalance: " << lPendingDarkBalance.toString();
    qDebug() << "lDarkBalance: " << lDarkBalance.toString();

    return lSuccess;
}

bool BitcoinBlockchainInterface::getUnspentTransactions(QList<BitcoinWallet> iWallets, QStringList &oTxids, QStringList &oValues, QStringList &oVouts, QStringList &oScriptPubKey, QList<QByteArray> &oPrivateKeys, int iConfirmations)
{
    QNetworkReply           *lReply;
    bool                    lSuccess = true;
    QEventLoop              lMyEventLoop;

    oTxids.clear();
    oValues.clear();
    oVouts.clear();
    oPrivateKeys.clear();

    for (auto w : iWallets) {
        lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addr/%2/utxo").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(QString(w.address())))));
        connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
        lMyEventLoop.exec();

        if (lReply->error() == QNetworkReply::NoError) {
            QByteArray      lReplyText = lReply->readAll();
            QList<QVariant> lMyArray = QJsonDocument::fromJson(lReplyText).toVariant().toList();

            for (auto utxo : lMyArray) {
                auto lTempMap = utxo.toMap();

//                qDebug() << lTempMap["address"].toString();

                if (lTempMap["confirmations"].toInt() >= iConfirmations) {
                    oValues << lTempMap["amount"].toString();
                    oTxids << lTempMap["txid"].toString();
                    oVouts << lTempMap["vout"].toString();
                    oScriptPubKey << lTempMap["scriptPubKey"].toString();
                    oPrivateKeys << w.wif();
                }
            }
            lSuccess = true;
        } else {
            qDebug() << "Error getting unspent transactions!";
            lSuccess = false;
        }
    }
    return lSuccess;
}

QByteArray BitcoinBlockchainInterface::createBitcoinTransaction(QList<BitcoinWallet> iInputWallets, QList<BitcoinWallet> iOutputs, QString iMinerFee, QList<QByteArray> &oPrivateKeys, QStringList &oTxids, QStringList &oVouts, QStringList &oScriptPubKey)
{
    QStringList         lTxids;
    QStringList         lValues;
    QStringList         lVouts;
    QStringList         lScriptPublicKey;
    QList<QByteArray>   lPrivateKeys;
    QList<int>          lIndices;
    QStringMath         lOutputTotal;
    QStringMath         lOutputBalance;
    QStringMath         lInputCounter = QStringMath("0.0");

    QString             lTransactionParams = "";
    QString             lInputParams = "";
    QString             lOutputParams = "";

    oPrivateKeys.clear();
    oVouts.clear();
    oTxids.clear();
    oScriptPubKey.clear();

    if (iInputWallets.size() == 0 || iOutputs.size() == 0) {
        qDebug() << "Error, no inputs or outputs";
        return QByteArray();
    }

    if (!getUnspentTransactions(iInputWallets, lTxids, lValues, lVouts, lScriptPublicKey, lPrivateKeys)) {
        qDebug() << "Error getting unspent transactions";
        return QByteArray();
    }

    for (auto outs : iOutputs) {
        lOutputTotal = lOutputTotal + outs.value();
        lOutputParams.append(QString("\"%1\":%2,").arg(QString(outs.address())).arg(outs.value()));
    }
    lOutputParams.remove(lOutputParams.size() - 1,1);

    for (int i = 0; i < lTxids.size(); i++) {
        if (QStringMath(lValues.at(i)) < lOutputTotal + iMinerFee) {
            lIndices.append(i);
            oPrivateKeys.append(lPrivateKeys.at(i));
            oTxids.append(lTxids.at(i));
            oVouts.append(lVouts.at(i));
            oScriptPubKey.append(lScriptPublicKey.at(i));
            lInputCounter = lInputCounter + lValues.at(i);
        } else if (QStringMath(lValues.at(i)) > lOutputTotal + iMinerFee) {
            lIndices.append(i);
            oPrivateKeys.append(lPrivateKeys.at(i));
            oTxids.append(lTxids.at(i));
            oVouts.append(lVouts.at(i));
            oScriptPubKey.append(lScriptPublicKey.at(i));
            lInputCounter = lInputCounter + lValues.at(i);
            break;
        }
    }

    lOutputBalance = lInputCounter - (lOutputTotal + iMinerFee);

    if (lOutputBalance > "0.0") {
        lOutputParams.append(QString(",\"%1\":%2").arg(QString(mActiveUser->getBrightWallet().address())).arg(lOutputBalance.toString()));
    }

    for (int i = 0; i < lIndices.size(); i++) {
        QJsonObject lTxin;
        lTxin["txid"] = lTxids[i];
        lTxin["vout"] = lVouts[i];

        lInputParams.append(QString("{\"txid\":\"%1\",\"vout\":%2},").arg(lTxids[i]).arg(lVouts[i]));
    }
    lInputParams.remove(lInputParams.size()-1,1);

    lTransactionParams = QString("[[%1],{%2}]").arg(lInputParams).arg(lOutputParams);

    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QNetworkRequest         lRequest(QUrl("http://test:test@24.234.113.55:8332"));
    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray lUrlData = QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"createrawtransaction\",\"params\":%1}").arg(lTransactionParams).toUtf8();


    qDebug() << QString(lUrlData).toUtf8().constData();

    lReply = mNetworkAccessManager->post(lRequest, lUrlData);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lResult = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lResult);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();

        return lMap["result"].toByteArray();
    } else {
        qDebug() << "Error.... Failed to create raw transaction";
        return QByteArray();
    }

    return lTransactionParams.toUtf8();
}

bool BitcoinBlockchainInterface::createBitcoinTransaction(QList<BitcoinWallet> iInputWallets, QMap<QString, QString> iOutputs, QString iMinerFee, QList<QByteArray> &oPrivateKeys, QStringList &oTxids, QStringList &oVouts, QStringList &oScriptPubKey, QByteArray &oRawTransaction)
{
    QEventLoop          lMyEventLoop;
    QNetworkReply       *lReply;
    QNetworkRequest     lRequest(QUrl("http://test:test@24.234.113.55:8332"));
    QByteArray          lUrlData;
    QStringList         lTxids;
    QStringList         lValues;
    QStringList         lVouts;
    QStringList         lScriptPublicKey;
    QList<QByteArray>   lPrivateKeys;
    QList<int>          lIndices;
    QStringMath         lOutputBalance;
    QStringMath         lInputCounter;

    QString             lTransactionParams = "";
    QString             lInputParams = "";
    QString             lOutputParams = "";

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    oRawTransaction.clear();
    oPrivateKeys.clear();
    oVouts.clear();
    oTxids.clear();
    oScriptPubKey.clear();

    if (!getUnspentTransactions(iInputWallets, lTxids, lValues, lVouts, lScriptPublicKey, lPrivateKeys)) {
        qDebug() << "Error getting unspent transactions";
        return false;
    }

    if (lTxids.count() == 0) {
        qDebug() << "Error, no unspent transactions";
        return false;
    }

    QStringMath lOutputTotal;
    for (auto lOutputKey : iOutputs.keys()) {
        lOutputTotal = lOutputTotal + iOutputs[lOutputKey];
        lOutputParams.append(QString("\"%1\":%2,").arg(lOutputKey).arg(iOutputs[lOutputKey]));
    }
    lOutputParams.remove(lOutputParams.size() - 1,1);

//    if (lOutputBalance > "0.0") {
//        lOutputParams.append(QString(",\"%1\":%2").arg(QString(mActiveUser->getBrightWallet().address())).arg(lOutputBalance.toString()));
//    }

    for (int i = 0; i < lTxids.size(); i++) {
        oPrivateKeys << lPrivateKeys[i];
        oTxids << lTxids[i];
        oVouts << lVouts[i];
        oScriptPubKey << lScriptPublicKey[i];

        lInputCounter = lInputCounter + lValues.at(i);
        lInputParams.append(QString("{\"txid\":\"%1\",\"vout\":%2},").arg(lTxids[i]).arg(lVouts[i]));

        if (lInputCounter >= lOutputTotal + iMinerFee) {
            break;
        }
    }
    lInputParams.remove(lInputParams.size()-1,1); // remove last comma
    lTransactionParams = QString("[[%1],{%2}]").arg(lInputParams).arg(lOutputParams);

    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    lUrlData = QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"createrawtransaction\",\"params\":%1}").arg(lTransactionParams).toUtf8();
    lReply = mNetworkAccessManager->post(lRequest, lUrlData);
    lMyEventLoop.exec();

    qDebug() << QString(lUrlData).toUtf8().constData();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lResult = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lResult);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();

        oRawTransaction = lMap["result"].toByteArray();
    } else {
        qDebug() << "Error.... Failed to create raw transaction : " << lReply->errorString();
        return false;
    }

    return true;
}

QString BitcoinBlockchainInterface::estimateMinerFee(int iInputs, int iOutputs, bool iRoundUp, int iBlocksToBeConfirmed)
{
    QEventLoop      lMyEventLoop;
    QNetworkReply   *lReply;
    bool            lSuccess = true;
    QStringMath     lEstimatedFee = QStringMath();

    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
    lReply = mNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/utils/estimatefee?nbBlocks=%2").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(iBlocksToBeConfirmed))));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lReplyText);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();

        double lMediumMinerFeesRate = lMap[QString("%1").arg(iBlocksToBeConfirmed)].toDouble();
        double lTransactionSizeEstimateInBytes = ((iInputs * 180) + (iOutputs * 34) + 10 );
        double lTransactionFee = lMediumMinerFeesRate * (lTransactionSizeEstimateInBytes);

        if (iRoundUp) {
            lEstimatedFee = QStringMath::roundUpToNearest0001(QString::number(lTransactionFee)).toString();
        } else {
            lEstimatedFee = QString::number(lTransactionFee);
        }
        qDebug() << "Estimated Fee Rate: " << lMediumMinerFeesRate << " Transaction Size: " << lTransactionSizeEstimateInBytes << " Estimated Fee: " << lEstimatedFee.toString();
    } else {
        qDebug() << "Error.... Fee estimation failed";
        lEstimatedFee = QStringMath();
    }

    return lEstimatedFee.toString();
}

bool BitcoinBlockchainInterface::signRawTransaction(QByteArray iRawTransaction, QByteArray &oSignedHex, QList<QByteArray> iPrivateKeys, QStringList iTxids, QStringList iVouts, QStringList iScriptPubKey)
{
    BitcoinWallet           lTempWallet = mActiveUser->getBrightWallet();
    bool                    lCompleteSigning = false;
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QNetworkRequest         lRequest(QUrl("http://test:test@24.234.113.55:8332"));
    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString lPrivateKeys = "";
    for (auto pv : iPrivateKeys) {
        lPrivateKeys.append(QString("\"%1\",").arg(QString(pv)));
    }
    lPrivateKeys.remove(lPrivateKeys.size()-1,1);

    QString lDependencies = "";
    for (int i = 0; i < iTxids.count(); i++) {
        lDependencies.append(QString("{\"txid\":\"%1\",\"vout\":%2,\"scriptPubKey\":\"%3\"},").arg(iTxids.at(i)).arg(iVouts.at(i)).arg(iScriptPubKey.at(i)));
    }
    lDependencies.remove(lDependencies.size()-1,1);

    //check without dependencies...
    lDependencies = "";

    QString lParameters = QString("[\"%1\",[%2],[%3]]").arg(QString(iRawTransaction)).arg(lDependencies).arg(lPrivateKeys);
    QByteArray lUrlData = (QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"signrawtransaction\",\"params\":%1}").arg(lParameters)).toUtf8();

    qDebug() << QString(lUrlData).toUtf8().constData();

    lReply = mNetworkAccessManager->post(lRequest, lUrlData);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lResult = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lResult);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();
        QVariantMap     lResultMap = lMap["result"].toMap();

        oSignedHex = lResultMap["hex"].toByteArray();
        lCompleteSigning = lResultMap["complete"].toBool();

        if (!lCompleteSigning) {
            qDebug() << "Transaction signing was not complete... : \n" << lResultMap["errors"].toByteArray() << "\n";
            return false;
        }
    } else {
        qDebug() << "Error.... Failed to sign raw transaction : " << lReply->errorString();
        return false;
    }

    return lCompleteSigning;
}

bool BitcoinBlockchainInterface::sendRawTransaction(QByteArray iSignedHex)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QNetworkRequest         lRequest(QUrl("http://test:test@24.234.113.55:8332"));
    connect(mNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QByteArray lUrlData = (QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"sendrawtransaction\",\"params\":[\"%1\"]}").arg(QString(iSignedHex))).toUtf8();

    lReply = mNetworkAccessManager->post(lRequest, lUrlData);
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lResult = lReply->readAll();
        QJsonDocument   lJsonReponse = QJsonDocument::fromJson(lResult);
        QJsonObject     lJsonObject = lJsonReponse.object();
        QVariantMap     lMap = lJsonObject.toVariantMap();
        QByteArray      lResultFromSend = lMap["result"].toByteArray();

        if (lResultFromSend.isEmpty() || lResultFromSend.isNull()) {
            qDebug() << "Error.... Failed to send raw transaction : no result or result from send...";
            return false;
        }
    } else {
        qDebug() << "Error.... Failed to send raw transaction : " << lReply->errorString();
        return false;
    }

    return true;
}
