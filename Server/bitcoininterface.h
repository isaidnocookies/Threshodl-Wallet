#ifndef BITCOININTERFACE_H
#define BITCOININTERFACE_H

#include <QObject>

class BitcoinInterface : public QObject
{
    Q_OBJECT
public:
    explicit BitcoinInterface(QObject *iParent = nullptr);

signals:

public slots:
    void createWallet();
};

#endif // BITCOININTERFACE_H
