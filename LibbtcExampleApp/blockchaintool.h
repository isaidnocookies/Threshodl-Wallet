#ifndef BLOCKCHAINTOOL_H
#define BLOCKCHAINTOOL_H

#include <QObject>

#include <btc/tx.h>

#include <btc/cstr.h>
#include <btc/ecc.h>
#include <btc/ecc_key.h>
#include <btc/script.h>
#include <btc/bip32.h>
#include <btc/base58.h>
#include <btc/chainparams.h>
#include <btc/protocol.h>
#include <btc/serialize.h>
#include <btc/tool.h>
#include <btc/tx.h>
#include <btc/utils.h>

#include "bitcoinwallet.h"

typedef struct myBtcTx {
    QList<QByteArray> inputTxids;
    QList<int> inputVouts;
    QList<QByteArray> inputScript;
    QList<QByteArray> inputPublicKey;
    QList<QString> inputAmount;

    QStringList outputAddress;
    QStringList outputAmount;
    QList<QByteArray> outputScript;
} myBtcTx;

class BlockchainTool : public QObject
{
    Q_OBJECT
public:
    explicit BlockchainTool(QObject *parent = nullptr);

    static BitcoinWallet createNewBitcoinWallet(BitcoinWallet::ChainType iChain);
    static bool createRawTransaction(QStringList iInputUtxids, QStringList iInputVout, QStringList iOutputAddresses, QStringList iOutputAmountsSatoshis, QByteArray &oRawTransaction, BitcoinWallet::ChainType iChainType);
    static bool createRawTransactionUsingBitcoinNode(QStringList iInputUtxids, QStringList iInputVouts, QStringList iOutputAddresses, QStringList iOutputAmountsInBTC, QByteArray &oRawTransaction);
    static bool checkAddressBalance(QString iAddress, QString &oBalance, QString &oPendingBalance);
    static bool getUnspentTransactions(QString iAddress, QStringList &oUtxids, QStringList &oVouts, QStringList &oAmounts);
    static bool decodeRawTransaction(QByteArray iRawHex, QString &oOutputText, myBtcTx &oTx, BitcoinWallet::ChainType iChainType);
    static bool decodeRawTransactionFromTx(btc_tx iTx, BitcoinWallet::ChainType iChainType, QString &oOutputText);
    static bool signRawTransaction(QByteArray iRawHex, QList<QByteArray> iPrivateKeys, BitcoinWallet::ChainType iChainType, QByteArray &oSignedTransactionHex);

    static bool signRawTransactionV2(QByteArray iRawHex, QList<QByteArray> iPrivateKeys, QList<QByteArray> iScripts, QList<QByteArray> iPublicKeys);

    static bool getPublicKeyFromAddress(QString iAddress, QByteArray &oPublicKey);
    static bool getPublicKeyScriptFromAddress(QString iAddress, QByteArray &oPublicKeyScript);
    static bool getAddressAndScriptFromUtxo(QString iUtxo, int iVout, QByteArray &oAddress, QByteArray &oScriptPubKey);
};

#endif // BLOCKCHAINTOOL_H
