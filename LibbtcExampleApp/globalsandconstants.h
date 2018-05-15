#ifndef GLOBALSANDCONSTANTS_H
#define GLOBALSANDCONSTANTS_H

#include <QString>

#include "bitcoinwallet.h"

#define BLOCKCYPER_API_URL "https://api.blockcypher.com/v1/btc/test3"

#define TEST_SERVER_IP_ADDRESS "wss://10.10.0.38:4431"
#define BITCOIN_NODE_IP_ADDRESS "http://threshodl:h0lyThr35h0dLb%407m%40n13@btcnode-test3.threebx.com:8332"

#define TEST_INSIGHT_BITCORE_IP_ADDRESS "https://test-insight.bitpay.com/api"
#define BLOCKEXPLORER_ADDRESS_LINK_BASE "https://testnet.blockexplorer.com/address/"

inline BitcoinWallet::ChainType currentChain() {    return BitcoinWallet::ChainType::TestNet; }

inline QString theOrganization() {                  return QString("isaidnocookies"); }
inline QString theApplicationName() {               return QString("blockchainSampleProject"); }
inline QString theDomain() {                        return QString("isaidnocookies.com"); }

inline QString walletSetting() {                    return QString("walletSetting"); }

inline QString inputUtxosSetting() {                return QString("inputUtxosSetting"); }
inline QString inputVoutSetting() {                 return QString("inputVoutSetting"); }
inline QString outputAddressSetting() {             return QString("outputAddressSetting"); }
inline QString outputAmountSetting() {              return QString("outputAmountSetting"); }

inline QString privateKeySetting() {                return QString("privateKeySetting"); }
inline QString rawTransactionForSigningSetting() {  return QString("rawTransactionForSigningSetting"); }

inline QString currentRawHex() {                    return QString("currentRawHex"); }

#endif // GLOBALSANDCONSTANTS_H
