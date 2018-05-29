#ifndef GLOBALSANDCONSTANTS_H
#define GLOBALSANDCONSTANTS_H

#include <QString>

// turn off and on the use of the blockchain for transactions between dark and bright
#define USE_BLOCKCHAIN true

#define TEST_SERVER_IP_ADDRESS          "wss://10.10.0.38:4431"
//#define TEST_SERVER_IP_ADDRESS          "wss://bluewallet.threebx.com:4431"
//#define TEST_SERVER_IP_ADDRESS          "wss://10.10.0.77:4431"

#define MY_WALLET_SERVER_ADDRESS        "http://10.10.0.71:3000"

#define BITCOIN_NODE_IP_ADDRESS         "http://threshodl:h0lyThr35h0dLb%407m%40n13@btcnode-test3.threebx.com:8332"
#define LITCOIN_NODE_DNS                "http://lite:388a74f00eb5a0064b57c5c048ccbb85@litenode.threebx.com:9332"
#define ETHEREUM_NODE_DNS               "http://gethnode.threebx.com:8545"
#define ZCASH_NODE_DNS                  "http://zcash:388a74f00eb5a0064b57c5c048ccbb85@zcashnode.threebx.com:8332"
#define DASH_NODE_DNS                   "http://dash:388a74f00eb5a0064b57c5c048ccbb85@dashnode.threebx.com:9998"

#define TEST_INSIGHT_BITCORE_IP_ADDRESS "https://test-insight.bitpay.com/api"
#define BLOCKEXPLORER_ADDRESS_LINK_BASE "https://testnet.blockexplorer.com/address/"

#define BITCOIN_AVERAGE_API_URL         "https://apiv2.bitcoinaverage.com"

enum CryptoChain {
    None            = 0x0,  /* invalid wallet */
    Main            = 0x1,
    TestNet         = 0x2,
    RegressionNet   = 0x3
};

namespace DataKeys {
    inline QString usernameDataKey()                    { return QString("usernameDataKey"); }
    inline QString publicKeyDataKey()                   { return QString("publicKeyDataKey"); }
    inline QString privateKeyDataKey()                  { return QString("privateKeyDataKey"); }
    inline QString walletsDataKey()                     { return QString("walletDataKey"); }

    inline QString shortNameDataKey()                   { return QString("shortNameDataKey"); }
    inline QString longNameDataKey()                    { return QString("longNameDataKey"); }
    inline QString isDark()                             { return QString("isDark"); }
    inline QString chainDataKey()                       { return QString("chainDataKey"); }
    inline QString marketValueDataKey()                 { return QString("marketValueDataKey"); }
    inline QString exchangeCurrencyDataKey()            { return QString("exchangeCurrencyDataKey"); }
    inline QString confirmedBalanceCurrencyDataKey()    { return QString("confirmedBalanceCurrencyDataKey"); }
    inline QString unconfirmedBalanceCurrencyDataKey()  { return QString("unconfirmedBalanceCurrencyDataKey"); }
}

namespace AppInfo {
    inline QString appDomain()                   { return QString("threshodl.com"); }
    inline QString appName()                     { return QString("Threshodl Beta"); }
    inline QString appOrganization()             { return QString("ThreeB"); }
}

namespace AppWallets {
    inline QStringList walletShortnames()       { return (QStringList() <<
                                                          "BTC" <<
                                                          "dBTC" <<
                                                          "LTC" <<
                                                          "DASH" <<
                                                          "ZEC"); }
    inline QStringList walletLongnames()        { return (QStringList() <<
                                                          "Bitcoin" <<
                                                          "Dark Bitcoin" <<
                                                          "Litecoin" <<
                                                          "Dash" <<
                                                          "ZCash"); }
}

#endif // GLOBALSANDCONSTANTS_H
