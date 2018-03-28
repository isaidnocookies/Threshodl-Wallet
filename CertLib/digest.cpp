#include "digest.h"

#include <openssl/evp.h>

Digest::Digest()
{

}

Digest::Digest(const QByteArray iData, enum HashTypes iHashType, bool iIsSigned)
    : mHashType( iHashType )
    , mDataIsSigned( iIsSigned )
    , mData( iData )
{

}

enum Digest::HashTypes Digest::hashType() const
{
    return mHashType;
}

void Digest::addData(const QByteArray iData)
{
    if( ! iData.isEmpty() )
        mData.append(iData);
}

bool Digest::sign(EncryptionKey *iKey)
{
    if( mDataIsSigned )  return false;
    if( ! iKey )            return false;
    if( ! iKey->toEVP() )   return false;

    bool            lRet    = false;
    EVP_MD_CTX *    lMDCTX  = nullptr;
    size_t          lELen   = 0;

    if( (lMDCTX = EVP_MD_CTX_create()) ) {

        if( 1 == EVP_DigestSignInit(lMDCTX, nullptr, _hashEngine(), nullptr, iKey->toEVP()) ) {
            if( 1 == EVP_DigestSignUpdate(lMDCTX, mData.constData(), mData.size()) ) {
                if( 1 == EVP_DigestSignFinal(lMDCTX, nullptr, &lELen) ) {
                    QByteArray lEData{static_cast<int>(lELen), static_cast<char>(0)};
                    if( 1 == EVP_DigestSignFinal(lMDCTX, reinterpret_cast<unsigned char *>(lEData.data()), &lELen ) ) {
                        mData = lEData;
                        lRet = mDataIsSigned = true;
                    }
                }
            }
        }

        EVP_MD_CTX_destroy(lMDCTX);
    }

    return lRet;
}

bool Digest::verify(EncryptionKey *iKey, const QByteArray iSignature)
{
    if( ! iKey )                return false;
    if( ! iKey->toEVP() )       return false;

    bool            lRet    = false;
    EVP_MD_CTX *    lMDCTX  = nullptr;

    if( (lMDCTX = EVP_MD_CTX_create()) ) {

        if(1 == EVP_DigestVerifyInit(lMDCTX, NULL, _hashEngine(), NULL, iKey->toEVP())) {
            if(1 == EVP_DigestVerifyUpdate(lMDCTX, mData.constData(), mData.size()) ) {
                if(1 == EVP_DigestVerifyFinal(lMDCTX, reinterpret_cast<const unsigned char *>(iSignature.constData()), iSignature.size())) {
                    lRet = true;
                }
            }
        }

        EVP_MD_CTX_destroy(lMDCTX);
    }

    return lRet;
}

QByteArray Digest::sign(EncryptionKey *iKey, const QByteArray iData, enum Digest::HashTypes iHashType)
{
    Digest  lDigest;
    lDigest.setHashType(iHashType);
    lDigest.addData(iData);
    if( lDigest.sign(iKey) ) {
        return lDigest.data();
    }
    return QByteArray();
}

bool Digest::verify(EncryptionKey *iKey, const QByteArray iData, const QByteArray iSignature, enum Digest::HashTypes iHashType)
{
    Digest lDigest;
    lDigest.setHashType(iHashType);
    lDigest.addData(iData);
    return lDigest.verify(iKey,iSignature);
}

const EVP_MD *Digest::_hashEngine() const
{
    switch( mHashType ) {
    case MD5:       return EVP_md5();
    case SHA1:      return EVP_sha1();
    case SHA224:    return EVP_sha224();
    case SHA256:    return EVP_sha256();
    case SHA384:    return EVP_sha384();
    case SHA512:    return EVP_sha512();
    }

    return nullptr;
}

bool Digest::isEncrypted() const
{
    return mDataIsSigned;
}

QByteArray Digest::data() const
{
    return mData;
}

void Digest::setHashType(enum Digest::HashTypes iHashType)
{
    mHashType = iHashType;
}
