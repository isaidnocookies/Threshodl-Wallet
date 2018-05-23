#ifndef GLOBALSANDCONSTANTS_H
#define GLOBALSANDCONSTANTS_H

#include <QString>

// turn off and on the use of the blockchain for transactions between dark and bright
#define USE_BLOCKCHAIN true

#define TEST_SERVER_IP_ADDRESS          "wss://10.10.0.38:4431"
//#define TEST_SERVER_IP_ADDRESS          "wss://bluewallet.threebx.com:4431"
//#define TEST_SERVER_IP_ADDRESS          "wss://10.10.0.77:4431"

#define BITCOIN_NODE_IP_ADDRESS         "http://threshodl:h0lyThr35h0dLb%407m%40n13@btcnode-test3.threebx.com:8332"
#define LITCOIN_NODE_DNS                "http://lite:388a74f00eb5a0064b57c5c048ccbb85@litenode.threebx.com:9332"
#define ETHEREUM_NODE_DNS               ""

#define TEST_INSIGHT_BITCORE_IP_ADDRESS "https://test-insight.bitpay.com/api"
#define BLOCKEXPLORER_ADDRESS_LINK_BASE "https://testnet.blockexplorer.com/address/"

namespace DataKeys {
    inline QString usernameDataKey()             { return QString("usernameDataKey"); }
    inline QString publicKeyDataKey()            { return QString("publicKeyDataKey"); }
    inline QString privateKeyDataKey()           { return QString("privateKeyDataKey"); }
}

namespace AppInfo {
    inline QString appDomain()                   { return QString("threshodl.com"); }
    inline QString appName()                     { return QString("Threshodl Beta"); }
    inline QString appOrganization()             { return QString("ThreeB"); }
}

#endif // GLOBALSANDCONSTANTS_H
