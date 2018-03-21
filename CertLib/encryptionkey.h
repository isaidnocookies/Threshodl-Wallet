#ifndef ENCRYPTIONKEY_H
#define ENCRYPTIONKEY_H

#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include <QByteArray>

class EncryptionKey
{
protected:
    RSA *               mRSA;
    EVP_PKEY *          mEVP;
    bool                mHavePublicKey;
    bool                mHavePrivateKey;

public:
    ~EncryptionKey();

    EncryptionKey( int iBits = 4096 );
    EncryptionKey( EVP_PKEY * iEVP );
    EncryptionKey( const QByteArray iPEM, bool iIsPrivateKey = true );

    bool havePublicKey() const
    { return mHavePublicKey; }

    bool havePrivateKey() const
    { return mHavePrivateKey; }

    QByteArray privateToPEM() const;
    QByteArray publicToPEM() const;
    
    static QByteArray privateToPEM( EVP_PKEY * iKey );
    static QByteArray publicToPEM( EVP_PKEY * iKey );

    EVP_PKEY * takeEVP()
    {
        EVP_PKEY * lEVP = mEVP;
        mEVP = NULL;
        return lEVP;
    }

    EVP_PKEY * toEVP() const
    { return mEVP; }
};

#endif // ENCRYPTIONKEY_H
