#include "encryptionkey.h"
#include "sslbio.h"

#include <QDebug>

EncryptionKey::~EncryptionKey()
{
    if( mRSA )
        RSA_free(mRSA);

    if( mEVP )
        EVP_PKEY_free(mEVP);
}

EncryptionKey::EncryptionKey(int iBits)
    : mRSA(RSA_generate_key(iBits,RSA_F4,NULL,NULL))
    , mEVP(EVP_PKEY_new())
    , mHavePublicKey(true), mHavePrivateKey(true)
{
    if (!EVP_PKEY_assign_RSA(mEVP,mRSA)) {
        qCritical() << "Internal RSA error!";
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());
    }
}

EncryptionKey::EncryptionKey(EVP_PKEY *iEVP)
    : mRSA(NULL), mEVP(iEVP)
    , mHavePublicKey(true), mHavePrivateKey(true)
{
    if( mEVP ) {
        mHavePublicKey = (publicToPEM().isEmpty()?false:true);
        mHavePrivateKey = (privateToPEM().isEmpty()?false:true);
    }else{
        mHavePublicKey = mHavePrivateKey = false;
    }
}

EncryptionKey::EncryptionKey(const QByteArray iPEM, bool iIsPrivateKey)
    : mRSA(NULL), mEVP(NULL)
    , mHavePublicKey(false), mHavePrivateKey(false)
{
    SSLBIO  lBIO(iPEM);

    if( iIsPrivateKey ) {
        mEVP = PEM_read_bio_PrivateKey(lBIO.getBIO(),NULL,NULL,NULL);
        if( mEVP ) mHavePrivateKey = true;
    }else{
        mEVP = PEM_read_bio_PUBKEY(lBIO.getBIO(),NULL,NULL,NULL);
        if( mEVP ) mHavePublicKey = true;
    }
}

QByteArray EncryptionKey::privateToPEM() const
{
    SSLBIO      lBIO;
    QByteArray  lPEM;

    if( mEVP && mHavePrivateKey ) {
        if( PEM_write_bio_PrivateKey(lBIO.getBIO(),mEVP,NULL,NULL,0,NULL,NULL) ) {
            lPEM = lBIO.readAll();
        }
    }

    return lPEM;
}

QByteArray EncryptionKey::publicToPEM() const
{
    SSLBIO      lBIO;
    QByteArray  lPEM;

    if( mEVP && mHavePublicKey ) {
        if( PEM_write_bio_PUBKEY(lBIO.getBIO(),mEVP) ) {
            lPEM = lBIO.readAll();
        }
    }

    return lPEM;
}

QByteArray EncryptionKey::privateToPEM(EVP_PKEY *iKey)
{
    QByteArray  lPEM;
    if( iKey ) {
        SSLBIO      lBIO;
        if( PEM_write_bio_PrivateKey(lBIO.getBIO(),iKey,NULL,NULL,0,NULL,NULL) ) {
            lPEM = lBIO.readAll();
        }
    }
    return lPEM;
}

QByteArray EncryptionKey::publicToPEM(EVP_PKEY *iKey)
{
    QByteArray  lPEM;
    if( iKey ) {
        SSLBIO      lBIO;
        if( PEM_write_bio_PUBKEY(lBIO.getBIO(),iKey) ) {
            lPEM = lBIO.readAll();
        }
    }
    return lPEM;
}
