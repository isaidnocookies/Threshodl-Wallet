#ifndef GLOBALSANDCONSTANTS_H
#define GLOBALSANDCONSTANTS_H

#include <QString>
#include <QStringList>
#include <QMap>

// turn off and on the use of the blockchain for transactions between dark and bright
#define USE_BLOCKCHAIN true

#define MY_WALLET_SERVER_ADDRESS "http://10.10.0.14:3333"
//#define MY_WALLET_SERVER_ADDRESS "https://app.threshodl.com:3333"
#define TASK_SERVER_URL "https://pyts.threebx.com"
#define POLLING_INTERVAL 60
#define THRESHODL_PASSWORD "threshodlpassword"
#define TURN_OFF_POLLING 0

namespace AppWallets {

    inline QMap<QString, QString> walletNames()         { return QMap<QString, QString> {
                                                                {"BTC",     "Bitcoin"},
                                                                {"tBTC",    "Testnet Bitcoin"},
//                                                                {"dBTC",    "Dark Bitcoin"},
                                                                {"dtBTC",   "Dark Testnet Bitcoin"},
//                                                                {"DASH",    "Dash"},
//                                                                {"tDASH",   "Testnet Dash"},
                                                                {"LTC",     "Litecoin"},
                                                                {"tLTC",    "Testnet Litecoin"},
                                                                {"dtLTC",   "Dark Testnet Litecoin"}
//                                                                {"ZEC",     "ZCash"}
//                                                                {"tZEC",    "Testnet ZCash"},
//                                                                {"BCH",     "Bitcoin Cash"},
//                                                                {"tBCH",    "Testnet Bitcoin Cash"},
//                                                                {"DOGE",    "Dogecoin"},
//                                                                {"tDOGE",   "Testnet Dogecoin"},
//                                                                {"ETH",     "Ethereum"},
//                                                                {"tETH",    "Testnet Ethereum"}
                                                            };
                                                        }
}

//https://chain.so/api
namespace EndpointUrls {
//    inline QMap<QString, QString> insightUrls()         { return QMap<QString, QString> {
//                                                                {"BTC",     "https://insight.bitpay.com/api"},
//                                                                {"tBTC",    "https://test-insight.bitpay.com/api"},
//                                                                {"DASH",    "https://insight.dash.org/insight-api"},
//                                                                {"tDASH",   "https://test.insight.dash.siampm.com/api"},
//                                                                {"LTC",     "https://insight.litecore.io/api"},
//                                                                {"tLTC",    "https://chain.so/api/v2"},
//                                                                {"tLTC",    "https://testnet.litecore.io/api"},
//                                                                {"ZEC",     "https://zcashnetwork.info/api"},
//                                                                {"tZEC",    "https://explorer.testnet.z.cash/api"},
//                                                                {"BCH",     "https://bch-insight.bitpay.com/api"},
//                                                                {"tBCH",    "https://test-bch-insight.bitpay.com/api"},
//                                                                {"DOGE",    "https://dogechain.info/api/v1"},
//                                                                {"tDOGE",   "https://chain.so/api/v2"},
//                                                                {"ETH",     ""},
//                                                                {"tETH",    ""}
//                                                            };
//                                                        }

    inline QMap<QString, QString> cryptoNodes()         { return QMap<QString, QString> {
                                                                {"BTC",     ""},
                                                                {"tBTC",    "http://threshodl:h0lyThr35h0dLb%407m%40n13@btcnode-test3.threebx.com:8332"},
                                                                {"DASH",    ""},
                                                                {"tDASH",   "http://dash:388a74f00eb5a0064b57c5c048ccbb85@dashnode.threebx.com:9998"},
                                                                {"LTC",     ""},
                                                                {"tLTC",    "http://lite:388a74f00eb5a0064b57c5c048ccbb85@litenode.threebx.com:9332"},
                                                                {"ZEC",     ""},
                                                                {"tZEC",    "http://zcash:388a74f00eb5a0064b57c5c048ccbb85@zcashnode.threebx.com:8332"},
                                                                {"BCH",     ""},
                                                                {"tBCH",    ""},
                                                                {"DOGE",    ""},
                                                                {"tDOGE",   ""},
                                                                {"ETH",     ""},
                                                                {"tETH",    "http://gethnode.threebx.com:8545"}
                                                            };
                                                        }
}

enum CryptoNetwork {
    None            = 0x0,  /* invalid wallet */
    Main            = 0x1,
    TestNet         = 0x2,
    RegressionNet   = 0x3
};

namespace ErrorCodes {
    enum DarkErrors {
        None = 0,
        NotEnoughChange = 1,
        NoMicroWallets = 2,
        TransferFailed = 3,
        EmailFailed = 4,
        AttachmentFailed = 5,
        InvalidAmount = 6
    };
}

namespace DataKeys {
    inline QString usernameDataKey()                    { return QString("usernameDataKey"); }
    inline QString recoverySeedDataKey()                { return QString("recoverySeedDataKey"); }
    inline QString publicKeyDataKey()                   { return QString("publicKeyDataKey"); }
    inline QString privateKeyDataKey()                  { return QString("privateKeyDataKey"); }
    inline QString walletsDataKey()                     { return QString("walletDataKey"); }
    inline QString darkWalletDataKey()                  { return QString("darkWalletDataKey"); }

    inline QString shortNameDataKey()                   { return QString("shortNameDataKey"); }
    inline QString longNameDataKey()                    { return QString("longNameDataKey"); }
    inline QString isDark()                             { return QString("isDark"); }
    inline QString chainDataKey()                       { return QString("chainDataKey"); }
    inline QString marketValueDataKey()                 { return QString("marketValueDataKey"); }
    inline QString exchangeCurrencyDataKey()            { return QString("exchangeCurrencyDataKey"); }
    inline QString confirmedBalanceCurrencyDataKey()    { return QString("confirmedBalanceCurrencyDataKey"); }
    inline QString unconfirmedBalanceCurrencyDataKey()  { return QString("unconfirmedBalanceCurrencyDataKey"); }

    inline QString walletConfirmedBalanceDataKey()      { return QString("walletConfirmedBalanceDataKey"); }
    inline QString walletUnconfirmedBalanceDataKey()    { return QString("walletUnconfirmedBalanceDataKey"); }

    inline QString passcodeDataKey()                    { return QString("passcodeDataKey"); }

    inline QString emailDataKey()                       { return QString("emailDataKey"); }
}

namespace AppInfo {
    inline QString appDomain()                   { return QString("threshodl.com"); }
    inline QString appName()                     { return QString("Threshodl Beta"); }
    inline QString appOrganization()             { return QString("ThreeB"); }
    inline QString myDateFormat()                { return QString("d.M.yy"); }
}

#endif // GLOBALSANDCONSTANTS_H
