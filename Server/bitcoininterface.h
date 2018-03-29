#ifndef BITCOININTERFACE_H
#define BITCOININTERFACE_H

#include <QByteArray>
#include <QString>
#include <QList>
#include <QMap>
#include <QSharedPointer>
#include <QObject>

#define kBTCChain_Test                  "Test"
#define kBTCChain_Main                  "Main"

#define kBTCAddressKey_P2PKH            "P2PKH"         /* Default / Classic BTC */
#define kBTCAddressKey_P2WPKH           "P2WPKH"
#define kBTCAddressKey_P2SH_P2WPKH      "P2SH+P2WPKH"   /* SEGWIT */

class BitcoinWallet
{
public:
    QByteArray                      Chain;
    QByteArray                      PrivateKey;
    QByteArray                      PublicKey;
    QByteArray                      Wif;
    QMap< QString, QByteArray >     Addresses;
};

typedef QSharedPointer<BitcoinWallet> BitcoinWalletRef;

class BitcoinInterface : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinInterface(QObject *iParent = nullptr);

    QList< BitcoinWalletRef > createWallets(int iWalletCount = 1, bool iTest = false);

signals:
    void walletCreated(BitcoinWalletRef iWallet);
    void walletFailedToCreate();

public slots:
    void createWallet(bool iTest = false);

private:
    BitcoinWalletRef    _createOneWallet(bool iTest);

    int                 mMaxWalletCreateFailureCount    = 10;
};

Q_DECLARE_METATYPE(BitcoinWalletRef)

#endif // BITCOININTERFACE_H
