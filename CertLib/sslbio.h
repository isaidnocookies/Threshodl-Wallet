#ifndef SSLBIO_H
#define SSLBIO_H

#include <openssl/pem.h>
#include <openssl/x509v3.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include <QByteArray>

class SSLBIO
{
protected:
    mutable BIO *   mBIO;
    bool            mValid;
    bool            mFreeOnDealloc;

public:
    SSLBIO();
    SSLBIO( const QByteArray iData );
    ~SSLBIO();

    bool freeBIOOnDealloc() const
    { return mFreeOnDealloc; }

    void setFreeBIOOnDealloc(bool iOn = true)
    { mFreeOnDealloc = iOn; }

    bool append( const QByteArray iData );

    int size() const
    { return static_cast<int>(BIO_ctrl_pending(mBIO)); }

    BIO * getBIO() const;
    QByteArray readAll() const;
};

#endif // SSLBIO_H
