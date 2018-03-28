#include "digest.h"
#include "certificate.h"

#include <QDebug>

int main()
{
    Certificate *   lCert1      = new Certificate;
    lCert1->setCADefaults();
    lCert1->selfSign();

    Certificate *   lCert2      = new Certificate;
    lCert2->setCADefaults();
    lCert2->selfSign();

    EncryptionKey * lPubKey1    = new EncryptionKey(lCert1->encryptionKey()->publicToPEM(),false);
    EncryptionKey * lPubKey2    = new EncryptionKey(lCert2->encryptionKey()->publicToPEM(),false);

    QByteArray      lMessage    = "Hello world";
    QByteArray      lSig        = Digest::sign(lCert1->encryptionKey(), lMessage);

    qDebug() << Digest::verify(lPubKey1, lMessage, lSig);   // Should return true
    qDebug() << Digest::verify(lPubKey2, lMessage, lSig);   // Should return false
    qDebug() << Digest::verify(lCert1->encryptionKey(), lMessage, lSig);   // Should return true
    qDebug() << Digest::verify(lCert2->encryptionKey(), lMessage, lSig);   // Should return false

    return 0;
}
