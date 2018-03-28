#include "bitcoininterface.h"

#include <QDebug>

//#include <btc/bip32.h>
#include <btc/chainparams.h>
//#include <btc/ecc.h>
//#include <btc/protocol.h>
//#include <btc/serialize.h>
#include <btc/tool.h>
//#include <btc/tx.h>
//#include <btc/utils.h>

BitcoinInterface::BitcoinInterface(QObject *iParent) : QObject(iParent)
{

}

void BitcoinInterface::createWallet(bool iTestNet)
{
    const btc_chainparams *     lChain                  = &btc_chainparams_main;
    size_t                      lPubSize                = 128;
    BitcoinWalletRef            lWallet                 = BitcoinWalletRef(new BitcoinWallet);
    QByteArray                  lP2PKH{128,0};
    QByteArray                  lP2WPKH{128,0};
    QByteArray                  lP2SH_P2WPKH{128,0};

    lWallet->PrivateKey = QByteArray{128,0};
    lWallet->PublicKey  = QByteArray{128,0};
    lWallet->Wif        = QByteArray{128,0};

    if( iTestNet ) {
        lChain = &btc_chainparams_test;
        lWallet->Chain = "TestNet";
    }else{
        lWallet->Chain = "Real";
    }

    if( gen_privatekey(lChain, lWallet->Wif.data(), 128, lWallet->PrivateKey.data() ) ) {
        if( pubkey_from_privatekey(lChain, lWallet->PrivateKey.data(), lWallet->PublicKey.data(), &lPubSize) ) {
            if( addresses_from_pubkey(lChain, lWallet->PublicKey.data(), lP2PKH.data(), lP2SH_P2WPKH.data(), lP2WPKH.data()) ) {
                lWallet->Addresses["P2PKH"]         = lP2PKH;
                lWallet->Addresses["P2WPKH"]        = lP2WPKH;
                lWallet->Addresses["P2SH+P2WPKH"]   = lP2SH_P2WPKH;
            }else{
                qWarning() << "";
            }
        }else{
            qWarning() << "";
        }
    }else{
        qWarning() << "";
    }
}
