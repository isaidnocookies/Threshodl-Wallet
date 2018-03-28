#ifndef BITCOININTERFACE_H
#define BITCOININTERFACE_H

#include <QByteArray>
#include <QString>
#include <QMap>
#include <QSharedPointer>
#include <QObject>

class BitcoinWallet
{
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

signals:

public slots:
    void createWallet(bool iTestNet = false);
};

#endif // BITCOININTERFACE_H
