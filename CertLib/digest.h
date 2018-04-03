#ifndef DIGEST_H
#define DIGEST_H

#include "encryptionkey.h"

class Digest
{
public:
    typedef enum HashTypes {
        MD5,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512
    } HashTypes;

    explicit Digest();
    explicit Digest( const QByteArray iData, HashTypes iHashType = SHA512, bool iIsSigned = false );

    HashTypes       hashType() const;
    bool            isEncrypted() const;
    QByteArray      data() const;       // Returns the signature after its signed or the raw data before the signing

    void setHashType( HashTypes iHashType = SHA512 );
    void addData( const QByteArray iData );

    bool sign( EncryptionKey * iKey );
    bool verify( EncryptionKey * iKey, const QByteArray iSignature );

    static QByteArray   sign( const QByteArray iKey, const QByteArray iData, HashTypes iHashType = SHA512, bool iPrivateKey = true );
    static QByteArray   sign( EncryptionKey * iKey, const QByteArray iData, HashTypes iHashType = SHA512 );

    static bool         verify( const QByteArray iKey, const QByteArray iData, const QByteArray iSignature, HashTypes iHashType = SHA512, bool iPrivateKey = false );
    static bool         verify( EncryptionKey * iKey, const QByteArray iData, const QByteArray iSignature, HashTypes iHashType = SHA512 );

private:
    const EVP_MD * _hashEngine() const;

    HashTypes       mHashType           = SHA512;
    bool            mDataIsSigned       = false;
    QByteArray      mData;
};

#endif // DIGEST_H
