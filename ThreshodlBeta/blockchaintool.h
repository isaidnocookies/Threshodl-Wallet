#ifndef BLOCKCHAINTOOL_H
#define BLOCKCHAINTOOL_H

#include <QString>

class BlockchainTool {

public:
    virtual bool sendTransaction();
    virtual bool createTransaction();
    virtual bool sendTransaction();
    virtual bool getAddressBalance(QString iAddress, QString &oBalance);

protected:
    QString mBlockchainType;
    QString mCryptoShortName;
    QString mNodeUrl;
}

#endif // BLOCKCHAINTOOL_H
