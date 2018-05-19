#include "blockchaintool.h"
#include "globalsandconstants.h"

#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

BlockchainTool::BlockchainTool(QObject *parent) : QObject(parent)
{

}

BitcoinWallet BlockchainTool::createNewBitcoinWallet(BitcoinWallet::ChainType iChain)
{
    BitcoinWallet lNewWallet = BitcoinWallet::createNewBitcoinWallet(iChain);

    qDebug() << "New Wallet Address: " << lNewWallet.address();
    qDebug() << "New Wallet Wif: " << lNewWallet.wif();
    qDebug() << "New Wallet Private Key: " << lNewWallet.privateKey();

    return lNewWallet;
}

bool BlockchainTool::createRawTransaction(QStringList iInputUtxids, QStringList iInputVout, QStringList iOutputAddresses, QStringList iOutputAmountsSatoshis, QByteArray &oRawTransaction, BitcoinWallet::ChainType iChainType)
{
    bool                        lSuccess;
    const btc_chainparams *     lChain;
    btc_tx*                     lTx = btc_tx_new();
    QByteArray                  lRawTransaction;

    lTx->version = 2;

    switch( iChainType ) {
    case BitcoinWallet::ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case BitcoinWallet::ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        lChain = &btc_chainparams_main;
        break;
    }

    if (iInputUtxids.size() != iInputVout.size()) {
        btc_tx_free(lTx);
        return false;
    }

    if (iOutputAddresses.size() != iOutputAmountsSatoshis.size()) {
        btc_tx_free(lTx);
        return false;
    }

    for (int i = 0; i < iInputUtxids.size(); i++) {
        btc_tx_in *lTxInput = btc_tx_in_new();

        QByteArray lUtxo = QString(iInputUtxids.at(i)).toUtf8();
        int lVout = QString(iInputVout.at(i)).toInt();

        lTxInput->script_sig = NULL;
        char* lPrevHash = lUtxo.data();
        utils_reverse_hex(lPrevHash, 64);
        lUtxo = lPrevHash; //reversed

        utils_uint256_sethex(lUtxo.data(), lTxInput->prevout.hash);
        lTxInput->prevout.n = lVout;

        vector_add(lTx->vin, lTxInput);
    }

    for (int i = 0; i < iOutputAddresses.size(); i++) {
        btc_bool lSucc = btc_tx_add_address_out(lTx, lChain, QString(iOutputAmountsSatoshis.at(i)).toInt(), QString(iOutputAddresses.at(i)).toUtf8().constData());
        lSuccess = lSucc;
    }

    cstring *txser = cstr_new_sz(1024);
    btc_tx_serialize(txser, lTx, false);
    char lHexbuf5[txser->len * 2 + 1];
    utils_bin_to_hex((unsigned char*)txser->str, txser->len, lHexbuf5);
    cstr_free(txser, true);

    oRawTransaction = lHexbuf5;

    QString lOutputText;
    lSuccess = decodeRawTransactionFromTx(*lTx, iChainType, lOutputText);
    qDebug() << QString(lOutputText).toUtf8().constData();

    return lSuccess;
}

bool BlockchainTool::createRawTransactionUsingBitcoinNode(QStringList iInputUtxids, QStringList iInputVouts, QStringList iOutputAddresses, QStringList iOutputAmountsInBTC, QByteArray &oRawTransaction)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QNetworkRequest         lRequest(QUrl(BITCOIN_NODE_IP_ADDRESS));
    QByteArray              lUrlData;
    QNetworkAccessManager   *lNetworkAccessManager = new QNetworkAccessManager();

    QString             lTransactionParams = "";
    QString             lInputParams = "";
    QString             lOutputParams = "";

    connect(lNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    if (iInputUtxids.count() == 0) {
        qDebug() << "Error, no unspent transactions";
        return false;
    }

    for (int i = 0; i < iOutputAddresses.count(); i++) {
        lOutputParams.append(QString("\"%1\":%2,").arg(iOutputAddresses.at(i)).arg(iOutputAmountsInBTC.at(i)));
    }
    lOutputParams.remove(lOutputParams.size() - 1,1);

    for (int i = 0; i < iInputUtxids.size(); i++) {
        lInputParams.append(QString("{\"txid\":\"%1\",\"vout\":%2},").arg(iInputUtxids[i]).arg(iInputVouts[i]));
    }
    lInputParams.remove(lInputParams.size()-1,1); // remove last comma

    lTransactionParams = QString("[[%1],{%2}]").arg(lInputParams).arg(lOutputParams);

    lRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    lUrlData = QString("{\"jsonrpc\":\"1.0\",\"id\":\"getinfo\",\"method\":\"createrawtransaction\",\"params\":%1}").arg(lTransactionParams).toUtf8();
    lReply = lNetworkAccessManager->post(lRequest, lUrlData);
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

bool BlockchainTool::checkAddressBalance(QString iAddress, QString &oBalance, QString &oPendingBalance)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    QList<BitcoinWallet>    lNewWallets;
    bool                    lSuccess = true;
    QNetworkAccessManager   *lNetworkAccessManager = new QNetworkAccessManager();

    connect(lNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    lReply = lNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addr/%2").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(iAddress))));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();

        QString         lPendingString = lMyMap["unconfirmedBalance"].toString();
        QString         lBalance = lMyMap["balance"].toString();

        oBalance = lBalance;
        oPendingBalance = lPendingString;
        lSuccess = true;
    } else {
        lSuccess = false;
    }

    return lSuccess;
}

bool BlockchainTool::getUnspentTransactions(QString iAddress, QStringList &oUtxids, QStringList &oVouts, QStringList &oAmounts)
{
    QNetworkReply           *lReply;
    bool                    lSuccess = true;
    QEventLoop              lMyEventLoop;
    QNetworkAccessManager   *lNetworkAccessManager = new QNetworkAccessManager();
    int                     lConfirmations = 2;

    oUtxids.clear();
    oAmounts.clear();
    oVouts.clear();

    lReply = lNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/addr/%2/utxo").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(QString(iAddress)))));
    connect(lNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        QList<QVariant> lMyArray = QJsonDocument::fromJson(lReplyText).toVariant().toList();

        for (auto utxo : lMyArray) {
            auto lTempMap = utxo.toMap();

            if (lTempMap["confirmations"].toInt() >= lConfirmations) {
                oAmounts << lTempMap["amount"].toString();
                oUtxids << lTempMap["txid"].toString();
                oVouts << lTempMap["vout"].toString();
            }
        }
        lSuccess = true;
    } else {
        qDebug() << "Error getting unspent transactions!";
        lSuccess = false;
    }

    return lSuccess;
}

bool BlockchainTool::decodeRawTransaction(QByteArray iRawHex, QString &oOutputText, myBtcTx &oTx, BitcoinWallet::ChainType iChainType)
{
    btc_tx*         lTx = btc_tx_new();
    bool            lSuccess = true;
    char*           lRawHex = iRawHex.data();
    myBtcTx         lMyTx;

    const btc_chainparams *     lChain;
    switch( iChainType ) {
    case BitcoinWallet::ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case BitcoinWallet::ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        lChain = &btc_chainparams_main;
        break;
    }

    uint8_t* lData_bin = reinterpret_cast<uint8_t*>(btc_malloc(strlen(lRawHex) / 2 + 1));

    int lOutlen = 0;
    utils_hex_to_bin(lRawHex, lData_bin, strlen(lRawHex), &lOutlen);
    if (!btc_tx_deserialize(lData_bin, lOutlen, lTx, NULL, true)) {
        oOutputText = "Invalid tx hex";

        free(lData_bin);
        btc_tx_free(lTx);

        return false;
    }

    int lInputIndex = 0;

    if ((size_t)lInputIndex >= lTx->vin->len) {

        free(lData_bin);
        btc_tx_free(lTx);

        return false;
    }

    oOutputText = "Input Information:\n\n";

    for (int i = 0; i < static_cast<int>(lTx->vin->len); i++) {
        btc_tx_in *lTx_in = reinterpret_cast<btc_tx_in*>(vector_idx(lTx->vin, i));

        char* lInHex = utils_uint8_to_hex(lTx_in->prevout.hash, sizeof(lTx_in->prevout.hash));
        utils_reverse_hex(lInHex, 64);

        oOutputText.append(QString("Utxo:   %1\nVout:   %2\n").arg(QString(lInHex)).arg(QString::number(lTx_in->prevout.n)));
        lMyTx.inputTxids << lInHex;
        lMyTx.inputVouts << lTx_in->prevout.n;
        btc_tx_in_free(lTx_in);
    }

    oOutputText.append("\n\nOutput Information:\n\n");

    for (int i = 0; i < static_cast<int>(lTx->vout->len); i++) {
        btc_tx_out *lTx_out = reinterpret_cast<btc_tx_out*>(vector_idx(lTx->vout, i));

        vector* lVec = vector_new(16, free);
        enum btc_tx_out_type lType = btc_script_classify(lTx_out->script_pubkey, lVec);

        if (lType == BTC_TX_PUBKEYHASH) {
            char* lScriptPubKey = utils_uint8_to_hex(reinterpret_cast<uint8_t*>(lTx_out->script_pubkey->str), lTx_out->script_pubkey->len);
            uint8_t* lHash160 = reinterpret_cast<uint8_t*>(vector_idx(lVec, 0));

            char* lAddress = new char[64];
            if (btc_p2pkh_addr_from_hash160(lHash160, lChain, lAddress, 64)) {
                  qDebug() << "Address from hash160 successful";

                  oOutputText.append(QString("Script PublicKey:   %1\n"
                                             "Address:   %2\n"
                                             "Output Amount:   %3\n").arg(lScriptPubKey).arg(lAddress).arg(QString::number(lTx_out->value)));

                  lMyTx.outputAddress.append(lAddress);
                  lMyTx.outputAmount.append(QString::number(lTx_out->value));
                  lMyTx.outputScript.append(lScriptPubKey);
            }
            delete[] lAddress;
        } else {
            oOutputText = "Transaction type not supported";
            lSuccess = false;
        }

        vector_free(lVec, true);
        btc_tx_out_free(lTx_out);
    }

    oTx = lMyTx;

    free(lData_bin);
    return lSuccess;
}

bool BlockchainTool::decodeRawTransactionFromTx(btc_tx iTx, BitcoinWallet::ChainType iChainType, QString &oOutputText)
{
    btc_tx*         lTx = &iTx;
    bool            lSuccess = true;

    const btc_chainparams *     lChain;
    switch( iChainType ) {
    case BitcoinWallet::ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case BitcoinWallet::ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        lChain = &btc_chainparams_main;
        break;
    }

    oOutputText = "Input Information:\n\n";

    for (int i = 0; i < static_cast<int>(lTx->vin->len); i++) {
        btc_tx_in *lTx_in = reinterpret_cast<btc_tx_in*>(vector_idx(lTx->vin, i));

        char* lInHex = utils_uint8_to_hex(lTx_in->prevout.hash, sizeof(lTx_in->prevout.hash));
        utils_reverse_hex(lInHex, 64);

        oOutputText.append(QString("Utxo:   %1\nVout:   %2\n").arg(QString(lInHex)).arg(QString::number(lTx_in->prevout.n)));
    }

    oOutputText.append("\n\nOutput Information:\n\n");

    for (int i = 0; i < static_cast<int>(lTx->vout->len); i++) {
        btc_tx_out *lTx_out = reinterpret_cast<btc_tx_out*>(vector_idx(lTx->vout, i));

        vector* lVec = vector_new(16, free);
        enum btc_tx_out_type lType = btc_script_classify(lTx_out->script_pubkey, lVec);

        if (lType == BTC_TX_PUBKEYHASH) {
            char* lScriptPubKey = utils_uint8_to_hex(reinterpret_cast<uint8_t*>(lTx_out->script_pubkey->str), lTx_out->script_pubkey->len);
            uint8_t* lHash160 = reinterpret_cast<uint8_t*>(vector_idx(lVec, 0));

            char* lAddress = new char[64];
            if (btc_p2pkh_addr_from_hash160(lHash160, lChain, lAddress, 64)) {
                  qDebug() << "Address from hash160 successful";

                  oOutputText.append(QString("Script PublicKey:   %1\n"
                                             "Address:   %2\n"
                                             "Output Amount:   %3\n").arg(lScriptPubKey).arg(lAddress).arg(QString::number(lTx_out->value)));
            }
            delete[] lAddress;
        } else {
            oOutputText = "Transaction type not supported";
            lSuccess = false;
        }

        vector_free(lVec, true);
        btc_tx_out_free(lTx_out);
    }

    return lSuccess;
}

bool BlockchainTool::signRawTransaction(QByteArray iRawHex, QList<QByteArray> iPrivateKeys, BitcoinWallet::ChainType iChainType, QByteArray &oSignedTransactionHex)
{
    bool                        lSuccess = true;
    const btc_chainparams *     lChain;
    char*                       lRawHex = iRawHex.data();
    btc_tx*                     lTx = btc_tx_new();

    switch( iChainType ) {
    case BitcoinWallet::ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case BitcoinWallet::ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        lChain = &btc_chainparams_main;
        break;
    }

    uint8_t* lData_bin = reinterpret_cast<uint8_t*>(btc_malloc(strlen(iRawHex) / 2 + 1));

    int lOutlen = 0;
    utils_hex_to_bin(lRawHex, lData_bin, strlen(lRawHex), &lOutlen);
    if (!btc_tx_deserialize(lData_bin, lOutlen, lTx, NULL, true)) {
        free(lData_bin);
        btc_tx_free(lTx);
        qDebug() << "Failed to deserialize transaction";

        return false;
    }

    int lInputIndex = 0;

    if ((size_t)lInputIndex >= lTx->vin->len) {
        free(lData_bin);
        btc_tx_free(lTx);

        return false;
    }

    if (static_cast<int>(lTx->vin->len) != iPrivateKeys.size()) {
        free(lData_bin);
        btc_tx_free(lTx);
        qDebug() << "Private keys amount and transaction inputs don't match";

        return false;
    }

//    btc_tx_in *lTx_in = reinterpret_cast<btc_tx_in*>(vector_idx(lTx->vin, 0)); // maybe?

    for (int i = 0; i < static_cast<int>(lTx->vin->len); i++) {
        btc_key key;
        btc_privkey_init(&key);
        if (btc_privkey_decode_wif(iPrivateKeys.at(i).constData(), lChain, &key)) {
            // decode wif successful. continue with signing.


        } else {
            qDebug() << "Failed to decode wif";
            free(lData_bin);

            lSuccess = false;
            break;
        }
    }

    if (lSuccess) {
        // serialize tx and output hex...
        cstring* lSigned_tx = cstr_new_sz(1024);
        btc_tx_serialize(lSigned_tx, lTx, true);

        char lSigned_tx_hex[lSigned_tx->len*2+1];
        utils_bin_to_hex((unsigned char *)lSigned_tx->str, lSigned_tx->len, lSigned_tx_hex);

        oSignedTransactionHex = lSigned_tx_hex;
        cstr_free(lSigned_tx, true);

        btc_tx_free(lTx);
    } else {
        qDebug() << "Failed to sign Transaction...";
        btc_tx_free(lTx);
        lSuccess = false;
    }

    return lSuccess;
}

bool BlockchainTool::signRawTransactionV2(QByteArray iRawHex, QList<QByteArray> iPrivateKeys, QList<QByteArray> iScripts, QList<QByteArray> iPublicKeys)
{
    bool lSuccess;
    int lSigHashType = 1;
    int lAmount = 0;

    const btc_chainparams *     lChain;

    switch( currentChain() ) {
    case BitcoinWallet::ChainType::TestNet:
        lChain = &btc_chainparams_test;
        break;
    case BitcoinWallet::ChainType::RegressionNet:
        lChain = &btc_chainparams_regtest;
        break;
    default:
        lChain = &btc_chainparams_main;
        break;
    }

    for (int i = 0; i < iScripts.size(); i++) {
        QByteArray lScriptHex = iScripts.at(i);
        auto lPrivateKey = iPrivateKeys.at(i).constData();
        if(iRawHex.isEmpty() || lScriptHex.isEmpty()) {
            qDebug() << "Missing / Invalid iRawHex or ScriptHex";
            return false;
        }

        if (strlen(iRawHex) > 1024*100) {
            qDebug() << "Tx too large (max 100kb)";
            return false;
        }

        //deserialize transaction
        btc_tx* tx = btc_tx_new();
        uint8_t* data_bin = reinterpret_cast<uint8_t*>(btc_malloc(strlen(iRawHex) / 2 + 1));
        int outlen = 0;
        utils_hex_to_bin(iRawHex, data_bin, strlen(iRawHex), &outlen);
        if (!btc_tx_deserialize(data_bin, outlen, tx, NULL, true)) {
            free(data_bin);
            btc_tx_free(tx);
            qDebug() << "Invalid tx hex";
        }
        free(data_bin);

        btc_tx_in *tx_in = reinterpret_cast<btc_tx_in*>(vector_idx(tx->vin, i));

        uint8_t script_data[strlen(lScriptHex) / 2 + 1];
        utils_hex_to_bin(lScriptHex, script_data, strlen(lScriptHex), &outlen);
        cstring* script = cstr_new_buf(script_data, outlen);

        uint256 sighash;
        memset(sighash, 0, sizeof(sighash));
        btc_tx_sighash(tx, script, i, lSigHashType, 0, SIGVERSION_BASE, sighash);

        char *hex = utils_uint8_to_hex(sighash, 32);
        utils_reverse_hex(hex, 64);

        enum btc_tx_out_type type = btc_script_classify(script, NULL);
        qDebug() << QString("script: %1").arg(QString(lScriptHex));
        qDebug() << QString("script-type: %1").arg(btc_tx_out_type_to_str(type));
        qDebug() << QString("inputindex: %1").arg(i);
        qDebug() << QString("sighashtype: %1").arg(lSigHashType);
        qDebug() << QString("hash: %1").arg(hex);

        // sign
        btc_bool sign = false;
        btc_key key;
        btc_privkey_init(&key);
        if (btc_privkey_decode_wif(lPrivateKey, lChain, &key)) {
            sign = true;
        }
        else {
            if (strlen(lPrivateKey) > 50) {
                btc_tx_free(tx);
                cstr_free(script, true);
                qDebug() << "Invalid Private Key";
                lSuccess = false;
            }
            qDebug() << "No private key provided, signing failed";
        }
        if (sign) {
            uint8_t sigcompact[64] = {0};
            int sigderlen = 74+1; //&hashtype
            uint8_t sigder_plus_hashtype[75] = {0};
            enum btc_tx_sign_result res = btc_tx_sign_input(tx, script, lAmount, &key, i, lSigHashType, sigcompact, sigder_plus_hashtype, &sigderlen);
            cstr_free(script, true);

            if (res != BTC_SIGN_OK) {
                qDebug() << QString("Sign error: %1").arg(btc_tx_sign_result_to_str(res));
                lSuccess = false;
            }

            char sigcompacthex[64*2+1] = {0};
            utils_bin_to_hex((unsigned char *)sigcompact, 64, sigcompacthex);

            char sigderhex[74*2+2+1]; //74 der, 2 hashtype, 1 nullbyte
            memset(sigderhex, 0, sizeof(sigderhex));
            utils_bin_to_hex((unsigned char *)sigder_plus_hashtype, sigderlen, sigderhex);

            cstring* signed_tx = cstr_new_sz(1024);
            btc_tx_serialize(signed_tx, tx, true);

            char signed_tx_hex[signed_tx->len*2+1];
            utils_bin_to_hex((unsigned char *)signed_tx->str, signed_tx->len, signed_tx_hex);
            printf("signed TX: %s\n", signed_tx_hex);
            cstr_free(signed_tx, true);
        }
        btc_tx_free(tx);
    }

    return lSuccess;
}

bool BlockchainTool::getPublicKeyFromAddress(QString iAddress, QByteArray &oPublicKey)
{

}

bool BlockchainTool::getPublicKeyScriptFromAddress(QString iAddress, QByteArray &oPublicKeyScript)
{

}

bool BlockchainTool::getAddressAndScriptFromUtxo(QString iUtxo, int iVout, QByteArray &oAddress, QByteArray &oScriptPubKey)
{
    QEventLoop              lMyEventLoop;
    QNetworkReply           *lReply;
    bool                    lSuccess = true;
    QNetworkAccessManager   *lNetworkAccessManager = new QNetworkAccessManager();

    connect(lNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), &lMyEventLoop, SLOT(quit()));

    lReply = lNetworkAccessManager->get(QNetworkRequest(QUrl(QString("%1/tx/%2").arg(TEST_INSIGHT_BITCORE_IP_ADDRESS).arg(iUtxo))));
    lMyEventLoop.exec();

    if (lReply->error() == QNetworkReply::NoError) {
        QByteArray      lReplyText = lReply->readAll();
        auto            lMyMap = QJsonDocument::fromJson(lReplyText).toVariant().toMap();
        auto            lMyArray = lMyMap["vout"].toList();

        auto            lVoutMap = lMyArray[iVout].toMap();
        auto            lScriptFields = lVoutMap["scriptPubKey"].toMap();
        auto            lAddresses = lScriptFields["addresses"].toList();
        auto            lScriptHex = lScriptFields["hex"].toByteArray();
        auto            lAddress = lAddresses.first().toByteArray();

        oAddress = lAddress;
        oScriptPubKey = lScriptHex;

        lSuccess = true;
    } else {
        lSuccess = false;
    }

    return lSuccess;
}
