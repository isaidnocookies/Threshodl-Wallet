#include "sslbio.h"

SSLBIO::SSLBIO()
{
    mBIO = BIO_new(BIO_s_mem());
    mFreeOnDealloc = mValid = true;
}

SSLBIO::SSLBIO(const QByteArray iData)
{
    mBIO = BIO_new(BIO_s_mem());
    mValid = append(iData);
    mFreeOnDealloc = true;
}

SSLBIO::~SSLBIO()
{
    if( mBIO && mFreeOnDealloc )
        BIO_free(mBIO);
}

bool SSLBIO::append(const QByteArray iData)
{
    int             lRead   = 0;
    int             lOffset = 0;
    const char *    lPtr    = iData.constData();
    int             lSize   = iData.size();

    while( lOffset < iData.size() ) {
        lRead = BIO_write(mBIO,lPtr+lOffset,lSize-lOffset);
        if( lRead < 0 ) break;
        lOffset += lRead;
    }

    if( lOffset >= lSize ) {
        return true;
    }

    return false;
}

BIO *SSLBIO::getBIO() const
{
    return mBIO;
}

QByteArray SSLBIO::readAll() const
{
    QByteArray  lData;
    char        lBuffer[4096];
    int         lRead;

    while( (lRead = BIO_read(mBIO,lBuffer,sizeof(lBuffer))) > 0 )
    { lData.append(lBuffer,lRead); };

    return lData;
}
