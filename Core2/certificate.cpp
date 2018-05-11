extern "C" {
    #include <openssl/ssl.h>
//    #include "deps/openssl/asn1.h"
}

#include "certificate.h"
#include "sslbio.h"

#include <QMutex>

#include <openssl/ssl.h>

static QMutex       _gInitLock;
static bool         _gNeedInit = true;
inline static void _gInit()
{
    _gInitLock.lock();
    if( _gNeedInit ) {
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_digests();
        _gNeedInit = false;
    }
    _gInitLock.unlock();
}

QStringList Certificate::_stringListExtensions(int iNID) const
{
    QStringList     lStringList;
    int             lIndex = -1;

    for(;;) {
        auto * lASN1String = reinterpret_cast<ASN1_IA5STRING*>(X509_get_ext_d2i(mX509,iNID,nullptr,&lIndex));
        if( ! lASN1String ) break;
        unsigned char * lUtf8 = nullptr;
        if( ASN1_STRING_to_UTF8(&lUtf8,lASN1String) ) {
            lStringList << QString::fromUtf8(reinterpret_cast<const char *>(lUtf8), ASN1_STRING_length(lASN1String) );
            OPENSSL_free(lUtf8);
        }
        ASN1_STRING_free(lASN1String);
    }

    return lStringList;
}

bool Certificate::_addExtension(int iNID, const char *iValue)
{
    X509_EXTENSION *    lX509Ext;
    X509V3_CTX          lX509Ctx;
    /* This sets the 'context' of the extensions. */
    /* No configuration database */
    X509V3_set_ctx_nodb(&lX509Ctx);
    /* Issuer and subject certs: both the target since it is self signed,
     * no request and no CRL
     */
    X509V3_set_ctx(&lX509Ctx, mX509, mX509, nullptr, nullptr, 0);
    lX509Ext = X509V3_EXT_conf_nid(nullptr, &lX509Ctx, iNID, const_cast<char *>(iValue));
    if (!lX509Ext)
        return false;

    X509_add_ext(mX509,lX509Ext,-1);
    X509_EXTENSION_free(lX509Ext);
    return true;
}

void Certificate::initializeLibrary()
{
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");

    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
    SSL_library_init();
    SSL_load_error_strings();
}

Certificate::~Certificate()
{
    if( mKey ) {
        // In Our case the EVP is owned by the crypto X509 so we just chuck it
        mKey->takeEVP();
        delete mKey;
    }

    if( mX509 )
        X509_free(mX509);
}

Certificate::Certificate(const QByteArray iPEM, bool iCSRPEM)
    : mKey(nullptr)
    , mX509(nullptr)
    , mMutable(iCSRPEM)
{
    _gInit();

    // First make the X509 Object
    SSLBIO  lBIO(iPEM);

    if( ! PEM_read_bio_X509(lBIO.getBIO(),&mX509,nullptr,nullptr) )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    // Now read the public key in
    EVP_PKEY * lKey = X509_get_pubkey(mX509);
    if( ! lKey )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    QByteArray lKeyPEM = EncryptionKey::publicToPEM(lKey);
    if( lKeyPEM.isEmpty() )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());
    
    mKey = new EncryptionKey(lKeyPEM,false);
}

Certificate::Certificate(X509 *iX509)
{
    _gInit();

    if( ! iX509 )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    mX509 = X509_dup(iX509);

    if( ! mX509 )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    EVP_PKEY * lKey = X509_get_pubkey(mX509);
    if( ! lKey )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    QByteArray lKeyPEM = EncryptionKey::publicToPEM(lKey);
    if( lKeyPEM.isEmpty() )
        throw std::runtime_error(QString("%1[%2]").arg(__FILE__).arg(__LINE__).toStdString());

    mKey = new EncryptionKey(lKeyPEM,false);
}

Certificate::Certificate(EncryptionKey *iKey)
    : mKey(iKey)
    , mX509(nullptr)
    , mMutable(true)
{
    _gInit();

    if( ! mKey )
        mKey = new EncryptionKey;

    mX509 = X509_new();

    X509_set_version(mX509,2);
    X509_set_pubkey(mX509,mKey->toEVP());
}

void Certificate::setSerialNumber(long iSerialNumber)
{
    ASN1_INTEGER_set(X509_get_serialNumber(mX509),iSerialNumber);
}

void Certificate::setExpiration(int iSecondsFromNow)
{
    X509_gmtime_adj(X509_get_notBefore(mX509),0);
    X509_gmtime_adj(X509_get_notAfter(mX509),iSecondsFromNow);
}

void Certificate::setIssuerName(QStringList iIssuerNameEntries)
{
    X509_NAME *     lX509Name   = X509_NAME_new();
    int             lPos;
    QString         lFieldName;
    QByteArray      lFieldNameBytes;
    QByteArray      lBytes;

    foreach( QString lEntry, iIssuerNameEntries ) {
        lPos = lEntry.indexOf("=");
        if( lPos < 0 ) continue;

        lFieldName      = lEntry.left(lPos);
        lEntry          = lEntry.mid(lPos+1);
        lFieldNameBytes = lFieldName.toUtf8();
        lBytes          = lEntry.toUtf8();

        X509_NAME_add_entry_by_txt( lX509Name, lFieldNameBytes.constData(), MBSTRING_ASC, reinterpret_cast<const unsigned char *>(lBytes.constData()), -1, -1, 0 );
    }

    X509_set_issuer_name(mX509,lX509Name);
}

void Certificate::setSubjectNames(QStringList iSubjectNameEntries)
{
    X509_NAME *     lX509Name = X509_get_subject_name(mX509);
    int             lPos;
    QString         lFieldName;
    QByteArray      lFieldNameBytes;
    QByteArray      lBytes;

    foreach( QString lEntry, iSubjectNameEntries ) {
        lPos = lEntry.indexOf("=");
        if( lPos < 0 ) continue;

        lFieldName      = lEntry.left(lPos);
        lEntry          = lEntry.mid(lPos+1);
        lFieldNameBytes = lFieldName.toUtf8();
        lBytes          = lEntry.toUtf8();

        X509_NAME_add_entry_by_txt( lX509Name, lFieldNameBytes.constData(), MBSTRING_ASC, reinterpret_cast<const unsigned char *>(lBytes.constData()), -1, -1, 0 );
    }

    X509_set_subject_name(mX509,lX509Name);
}

void Certificate::addSubjectName(const QString iSubjectName)
{
    X509_NAME *     lX509Name = X509_get_subject_name(mX509);
    int             lPos;
    QString         lFieldName;
    QByteArray      lFieldNameBytes;
    QByteArray      lBytes;
    QString         lSubjectName = iSubjectName;

    lPos = lSubjectName.indexOf("=");
    if( lPos < 0 ) return;

    lFieldName      = lSubjectName.left(lPos);
    lSubjectName    = lSubjectName.mid(lPos+1);
    lFieldNameBytes = lFieldName.toUtf8();
    lBytes          = lSubjectName.toUtf8();

    X509_NAME_add_entry_by_txt( lX509Name, lFieldNameBytes.constData(), MBSTRING_ASC, reinterpret_cast<const unsigned char *>(lBytes.constData()), -1, -1, 0 );

    X509_set_subject_name(mX509,lX509Name);
}

void Certificate::addServerName(const QString iServerName)
{
//     QByteArray  lUtf8 = iServerName.toUtf8();
//     _addExtension(NID_netscape_ssl_server_name,lUtf8.constData());
    addSubjectName(QString("CN=%1").arg(iServerName));
}

void Certificate::addComment(const QString iComment)
{
    QByteArray lUtf8 = iComment.toUtf8();
    _addExtension(NID_netscape_comment, lUtf8.constData());
}

void Certificate::addComment(const QByteArray iComment)
{
    _addExtension(NID_netscape_comment, iComment.constData());
}

void Certificate::addCommonName(const QString iCommonName)
{
    QByteArray  lUtf8 = iCommonName.toUtf8();
    _addExtension(NID_commonName,lUtf8.constData());
}

void Certificate::setCAExtensions(unsigned int iSigningDepth)
{
//    _addExtension(NID_basic_constraints,"critical,CA:TRUE");

    char lConstraints[256];
    memset( lConstraints, 0, sizeof(lConstraints) );
    snprintf( lConstraints, sizeof(lConstraints), "critical,CA:TRUE,pathlen:%u", iSigningDepth );

    _addExtension(NID_basic_constraints,lConstraints);
    _addExtension(NID_key_usage, "critical,keyCertSign,cRLSign,digitalSignature");
    _addExtension(NID_subject_key_identifier, "hash");
//    _addExtension(NID_netscape_cert_type, "sslCA");
}

void Certificate::setServerExtensions()
{
//    X509_set_version(mX509,0);
    _addExtension(NID_subject_key_identifier, "hash");
    _addExtension(NID_netscape_cert_type, "server,client");
//    _addExtension(NID_basic_constraints,"critical,CA:FALSE,pathlen:0");
    _addExtension(NID_ext_key_usage,"serverAuth,clientAuth");
    _addExtension(NID_key_usage,"digitalSignature");
    // NS_SSL_CLIENT
}

bool Certificate::verify(EncryptionKey *iSigningKey, bool iExpireCheck)
{
    if( ! iExpireCheck || ! expired() ) {
        if( iSigningKey ) {
            EVP_PKEY * lEVP = iSigningKey->toEVP();
            if( lEVP ) {
                if( 1 == X509_verify( mX509, lEVP) )
                    return true;
            }
        }
    }

    return false;
}

bool Certificate::selfVerify()
{ return verify(mKey); }

bool Certificate::sign(EncryptionKey *iSigningKey, enum HashTypes iHashType)
{
    const EVP_MD *  lHT = nullptr;

    switch( iHashType ) {
    case MD5:
        lHT = EVP_md5();
        break;

    case SHA1:
        lHT = EVP_sha1();
        break;

    case SHA224:
        lHT = EVP_sha224();
        break;

    case SHA256:
        lHT = EVP_sha256();
        break;

    case SHA384:
        lHT = EVP_sha384();
        break;

    case SHA512:
        lHT = EVP_sha512();
        break;
    }

    if( mMutable && iSigningKey && iSigningKey->havePrivateKey() ) {
        if( lHT && X509_sign(mX509,iSigningKey->toEVP(),lHT) ) {
            mMutable = false;
            return true;
        }
    }
    return false;
}

bool Certificate::selfSign(enum HashTypes iHashType)
{ return sign(mKey,iHashType); }

long Certificate::serialNumber() const
{ return ASN1_INTEGER_get(X509_get_serialNumber(mX509)); }

QStringList Certificate::comments() const
{ return _stringListExtensions(NID_netscape_comment); }

QStringList Certificate::serverNames() const
{ return _stringListExtensions(NID_netscape_ssl_server_name); }

QStringList Certificate::subjectName() const
{
    QStringList     lResult;
    X509_NAME *     lName   = X509_get_subject_name(mX509);

    if( lName ) {
        int lCount = X509_NAME_entry_count(lName);
        for( int lIndex = 0; lIndex < lCount; lIndex++ ) {
            X509_NAME_ENTRY * lEntry = X509_NAME_get_entry(lName,lIndex);
            if( lEntry ) {

                QString lFieldName;
                char lFieldNameC[128];
                memset(lFieldNameC,0,sizeof(lFieldNameC));

                ASN1_OBJECT * lObject = X509_NAME_ENTRY_get_object(lEntry);
                if( lObject ) {
                    if( OBJ_obj2txt(lFieldNameC,(static_cast<int>(sizeof(lFieldNameC)-1)),lObject,0) ) {
                        lFieldName = QString("%1=").arg(QString::fromUtf8(lFieldNameC));
                    }
                    // ASN1_OBJECT_free(lObject);
                }

                ASN1_STRING * lString = X509_NAME_ENTRY_get_data(lEntry);
                if( lString ) {
                    unsigned char * lUtf8 = nullptr;
                    if( ASN1_STRING_to_UTF8(&lUtf8,lString) ) {
                        lResult << QString("%1%2").arg(lFieldName).arg(QString::fromUtf8(reinterpret_cast<const char *>(lUtf8), ASN1_STRING_length(lString) ));
                        OPENSSL_free(lUtf8);
                    }
                    // ASN1_STRING_free(lString);
                }
            }
        }
    }

    return lResult;
}

QStringList Certificate::issuerName() const
{
    QStringList     lResult;
    X509_NAME *     lName   = X509_get_issuer_name(mX509);

    if( lName ) {
        int lCount = X509_NAME_entry_count(lName);
        for( int lIndex = 0; lIndex < lCount; lIndex++ ) {
            X509_NAME_ENTRY * lEntry = X509_NAME_get_entry(lName,lIndex);
            if( lEntry ) {

                QString lFieldName;
                char lFieldNameC[128];
                memset(lFieldNameC,0,sizeof(lFieldNameC));

                ASN1_OBJECT * lObject = X509_NAME_ENTRY_get_object(lEntry);
                if( lObject ) {
                    if( OBJ_obj2txt(lFieldNameC,(static_cast<int>(sizeof(lFieldNameC)-1)),lObject,0) ) {
                        lFieldName = QString("%1=").arg(QString::fromUtf8(lFieldNameC));
                    }
                }

                ASN1_STRING * lString = X509_NAME_ENTRY_get_data(lEntry);
                if( lString ) {
                    unsigned char * lUtf8 = nullptr;
                    if( ASN1_STRING_to_UTF8(&lUtf8,lString) ) {
                        lResult << QString("%1%2").arg(lFieldName).arg(QString::fromUtf8(reinterpret_cast<const char *>(lUtf8), ASN1_STRING_length(lString) ));
                        OPENSSL_free(lUtf8);
                    }
                }
            }
        }
    }

    return lResult;
}

bool Certificate::expired() const
{
    QDateTime   lDT     = QDateTime::currentDateTimeUtc();

    ASN1_TIME * lX509Time   = X509_get_notAfter(mX509);
    ASN1_TIME * lNowTime    = ASN1_TIME_set(nullptr, static_cast<time_t>(lDT.toTime_t()));
    int         lDays       = 0;
    int         lSeconds    = 0;
    bool        lResult     = true;

    if( ASN1_TIME_diff(&lDays,&lSeconds,lNowTime,lX509Time) ) {
        if( lDays >= 0 && lSeconds >= 0 )
            lResult = false;
    }

    return lResult;
}

QDateTime Certificate::expiration() const
{
    QDateTime   lDT     = QDateTime::currentDateTimeUtc();

    ASN1_TIME * lX509Time   = X509_get_notAfter(mX509);
    ASN1_TIME * lNowTime    = ASN1_TIME_set(nullptr,static_cast<time_t>(lDT.toTime_t()));
    int         lDays       = 0;
    int         lSeconds    = 0;

    if( ASN1_TIME_diff(&lDays,&lSeconds,lNowTime,lX509Time) ) {
        lDT = lDT.addDays(lDays);
        lDT = lDT.addSecs(lSeconds);
        return lDT;
    }

    return QDateTime::fromMSecsSinceEpoch(0);
}

QByteArray Certificate::toPEM()
{
    SSLBIO  lBIO;

    if( mMutable ) {
        // Need to make it immutable now - self sign
        if( ! selfSign() )
            return QByteArray();
    }

    if( ! mMutable && PEM_write_bio_X509(lBIO.getBIO(),mX509) ) {
        return lBIO.readAll();
    }
    return QByteArray();
}

QByteArray Certificate::toPEM() const
{
    SSLBIO  lBIO;

    if( ! mMutable && PEM_write_bio_X509(lBIO.getBIO(),mX509) ) {
        return lBIO.readAll();
    }
    return QByteArray();
}
