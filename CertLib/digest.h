#ifndef DIGEST_H
#define DIGEST_H

#include "encryptionkey.h"

class Digest
{
public:
    enum HashTypes {
        MD5,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512
    } HashTypes;

    explicit Digest();
    explicit Digest( const QByteArray iData, enum HashTypes iHashType = SHA512, bool iIsSigned = false );

    enum HashTypes  hashType() const;
    bool            isEncrypted() const;
    QByteArray      data() const;       // Returns the signature after its signed or the raw data before the signing

    void setHashType( enum HashTypes iHashType = SHA512 );
    void addData( const QByteArray iData );

    bool sign( EncryptionKey * iKey );
    bool verify( EncryptionKey * iKey );

    static QByteArray   sign( EncryptionKey * iKey, const QByteArray iData, enum HashTypes iHashType = SHA512 );
    static bool         verify( EncryptionKey * iKey, const QByteArray iData, enum HashTypes iHashType = SHA512 );

private:
    const EVP_MD * _hashEngine() const;

    enum HashTypes  mHashType           = SHA512;
    bool            mDataIsSigned       = false;
    QByteArray      mData;
};

#endif // DIGEST_H
