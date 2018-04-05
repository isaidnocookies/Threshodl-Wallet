#include "bitcoininterface.h"

#include <QDebug>

extern "C" {
//#include <btc/bip32.h>
#include <btc/chainparams.h>
#include <btc/ecc.h>
//#include <btc/protocol.h>
//#include <btc/serialize.h>
#include <btc/tool.h>
//#include <btc/tx.h>
//#include <btc/utils.h>
}

#define     kBTCStringBufferSize    128

BitcoinInterface::BitcoinInterface(QObject *iParent) : QObject(iParent)
{
    btc_ecc_start();
    qRegisterMetaType<BitcoinWalletRef>();
}

QList<BitcoinWalletRef> BitcoinInterface::createWallets(int iWalletCount, bool iTest)
{
    QList<BitcoinWalletRef>     lWallets;
    int                         lFailureCount   = 0;

    for( int lIndex = 0; lIndex < iWalletCount; lIndex++ ) {
        auto lWallet = _createOneWallet(iTest);

        if( lWallet ) {
            lWallets << lWallet;
        }else{
            lFailureCount++;
            if( lFailureCount < mMaxWalletCreateFailureCount )
                lIndex--;
        }
    }

    return lWallets;
}

void BitcoinInterface::createWallet(bool iTest)
{
    auto lWallet = _createOneWallet(iTest);
    if( lWallet )
        emit walletCreated(lWallet);
    else
        emit walletFailedToCreate();
}

BitcoinWalletRef BitcoinInterface::_createOneWallet(bool iTest)
{
    const btc_chainparams *     lChain                  = &btc_chainparams_main;
    size_t                      lPubSize                = kBTCStringBufferSize;
    BitcoinWalletRef            lWallet                 = BitcoinWalletRef(new BitcoinWallet);
    QByteArray                  lP2PKH{kBTCStringBufferSize,0};
    QByteArray                  lP2WPKH{kBTCStringBufferSize,0};
    QByteArray                  lP2SH_P2WPKH{kBTCStringBufferSize,0};

    lWallet->PrivateKey = QByteArray{kBTCStringBufferSize,0};
    lWallet->PublicKey  = QByteArray{kBTCStringBufferSize,0};
    lWallet->Wif        = QByteArray{kBTCStringBufferSize,0};

    if( iTest ) {
        lChain = &btc_chainparams_test;
        lWallet->Chain = kBTCChain_Test;
    }else{
        lWallet->Chain = kBTCChain_Main;
    }

    if( gen_privatekey(lChain, lWallet->Wif.data(), lWallet->Wif.size(), lWallet->PrivateKey.data() ) ) {
        if( pubkey_from_privatekey(lChain, lWallet->Wif.data(), lWallet->PublicKey.data(), &lPubSize) ) {
            if( addresses_from_pubkey(lChain, lWallet->PublicKey.data(), lP2PKH.data(), lP2SH_P2WPKH.data(), lP2WPKH.data()) ) {

                // Cleanup the sizes of the buffer
                lWallet->Wif.resize(qstrlen(lWallet->Wif.constData()) + 1);
                lWallet->PrivateKey.resize(qstrlen(lWallet->PrivateKey.constData()) + 1);
                lWallet->PublicKey.resize(qstrlen(lWallet->PublicKey.constData()) + 1);
                lP2PKH.resize(qstrlen(lP2PKH.constData()) + 1);
                lP2WPKH.resize(qstrlen(lP2WPKH.constData()) + 1);
                lP2SH_P2WPKH.resize(qstrlen(lP2SH_P2WPKH.constData()) + 1);

                lWallet->Addresses[kBTCAddressKey_P2PKH]        = lP2PKH;
                lWallet->Addresses[kBTCAddressKey_P2WPKH]       = lP2WPKH;
                lWallet->Addresses[kBTCAddressKey_P2SH_P2WPKH]  = lP2SH_P2WPKH;

                return lWallet;
            }else{
                qWarning() << "Failed to extract new BTC addresses for a new BTC public key for a new BTC wallet.";
            }
        }else{
            qWarning() << "Failed to extract a public key from a private key for a new BTC wallet.";
        }
    }else{
        qWarning() << "Failed to generate a new private key and wif for a new BTC wallet.";
    }

    return BitcoinWalletRef();
}
