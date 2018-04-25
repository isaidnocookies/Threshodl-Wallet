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

bool BitcoinBlockchainInterface::getUnspentTransactions(QList<BitcoinWallet> iWallets, QStringList &oTxids, QStringList &oValues, QStringList &oVouts, QList<QByteArray> &oPrivateKeys, int iConfirmations)
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
//                qDebug() << lTempMap["scriptPubKey"].toString();

                if (lTempMap["confirmations"].toInt() >= iConfirmations) {
                    oValues.append(lTempMap["amount"].toString());
                    oTxids.append(lTempMap["txid"].toString());
                    oVouts.append(lTempMap["vout"].toString());
                    oPrivateKeys.append(w.wif());
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

QByteArray BitcoinBlockchainInterface::createBitcoinTransaction(QList<BitcoinWallet> iInputWallets, QList<BitcoinWallet> iOutputs, QString iMinerFee, QList<QByteArray> &oPrivateKeys)
{
    QStringList         lTxids;
    QStringList         lValues;
    QStringList         lVouts;
    QList<QByteArray>   lPrivateKeys;
    QList<int>          lIndices;
    QStringMath         lOutputTotal;
    QStringMath         lOutputBalance;
    QStringMath         lInputCounter = QStringMath("0.0");

    QString             lTransactionParams = "";
    QString             lInputParams = "";
    QString             lOutputParams = "";

    if (iInputWallets.size() == 0 || iOutputs.size() == 0) {
        qDebug() << "Error, no inputs or outputs";
        return QByteArray();
    }

    if (!getUnspentTransactions(iInputWallets, lTxids, lValues, lVouts, lPrivateKeys)) {
        qDebug() << "Error getting unspent transactions";
        return QByteArray();
    }

    for (int i = 0; i < lTxids.size(); i++) {
        if (QStringMath(lValues.at(i)) < lOutputTotal + iMinerFee) {
            lIndices.append(i);
            oPrivateKeys.append(lPrivateKeys.at(i));
            lInputCounter = lInputCounter + lValues.at(i);
        } else if (QStringMath(lValues.at(i)) > lOutputTotal + iMinerFee) {
            lIndices.append(i);
            oPrivateKeys.append(lPrivateKeys.at(i));
            lInputCounter = lInputCounter + lValues.at(i);
            break;
        }
    }

    if (lInputCounter < lOutputTotal + iMinerFee) {
        qDebug() << "Not enough bitcoin...";
        return QByteArray();
    }

    for (auto outs : iOutputs) {
        lOutputTotal = lOutputTotal + outs.value();
        lOutputParams.append(QString("\"%1\":%2,").arg(QString(outs.address())).arg(outs.value()));
    }

    lOutputBalance = lInputCounter -  (lOutputTotal + iMinerFee);
    lOutputParams.append(QString("\"%1\":%2").arg(QString(mActiveUser->getBrightWallet().address())).arg(lOutputBalance.toString()));

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

bool BitcoinBlockchainInterface::signRawTransaction(QByteArray iRawTransaction, QByteArray &oSignedHex, QList<QByteArray> iPrivateKeys)
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

    QString lParameters = QString("[\"%1\",[],[%2]]").arg(QString(iRawTransaction)).arg(lPrivateKeys);
    QByteArray lUrlData = (QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"signrawtransaction\",\"params\":%1}").arg(lParameters)).toUtf8();

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
    } else {
        qDebug() << "Error.... Failed to create raw transaction";
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
            return false;
        }
    } else {
        qDebug() << "Error.... Failed to create raw transaction";
        return false;
    }

    return true;
}
