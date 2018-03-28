#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "encryptionkey.h"

#include <openssl/conf.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/x509v3.h>

#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>

#define kSecondsInADay      86400
#define kSecondsInAYear     31536000

class Certificate
{
protected:
    EncryptionKey *     mKey;
    X509 *              mX509;
    bool                mMutable;

    QStringList _stringListExtensions( int iNID ) const;

    bool _addExtension(int iNID, const char * iValue);

public:
    static void initializeLibrary();

    ~Certificate();
    Certificate(const QByteArray iPEM, bool iCSRPEM = false);
    Certificate(X509 * iX509);
    Certificate(EncryptionKey *iKey = nullptr);

    enum HashTypes {
        MD5,
        SHA1,
        SHA224,
        SHA256,
        SHA384,
        SHA512
    } HashTypes;

    void setSerialNumber(long iSerialNumber = 1 );
    void setExpiration( int iSecondsFromNow = kSecondsInAYear );
    void setIssuerName(QStringList iIssuerNameEntries );
    void setSubjectNames(QStringList iSubjectNameEntries );
    void addSubjectName(const QString iSubjectName);
    void addServerName(const QString iServerName);
    void addComment(const QString iComment);
    void addComment(const QByteArray iComment);
    void addCommonName(const QString iCommonName);
    void setCAExtensions(unsigned int iSigningDepth = 0);
    void setServerExtensions();

    void setCADefaults()
    { setSerialNumber(); setExpiration(); setCAExtensions(); }

    void setServerDefaults()
    { setSerialNumber(); setExpiration(); setServerExtensions(); }

    bool verify(EncryptionKey * iSigningKey, bool iExpireCheck = false);
    bool selfVerify();

    bool sign(EncryptionKey * iSigningKey, enum HashTypes iHashType = SHA512);
    bool selfSign(enum HashTypes iHashType = SHA512);

    long serialNumber() const;
    QStringList comments() const;
    QStringList serverNames() const;
    QStringList subjectName() const;
    QStringList issuerName() const;
    bool expired() const;
    QDateTime expiration() const;

    EncryptionKey * encryptionKey() const   // Returns an EncryptionKey object
    { return mKey; }

    X509 * toX509() const       // Returns the internal OpenSSL data object
    { return mX509; }

    QByteArray toPEM();         // Will self sign if needed
    QByteArray toPEM() const;   // Will fail if not signed
};

#endif // CERTIFICATE_H
