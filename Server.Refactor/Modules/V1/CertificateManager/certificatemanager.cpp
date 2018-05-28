#include "certificatemanager.h"
#include "app.h"

#include <QDateTime>
#include <QDebug>
#include <QFile>

static CertificateManagerML _gRegisterModuleLinker;

CertificateManagerML::CertificateManagerML()
{
    ModuleLinker::registerModule(QStringLiteral("CertificateManager-1"),CertificateManagerML::creator,CertificateManagerML::doInit,CertificateManagerML::start,CertificateManagerML::startInOwnThread);
}

void *CertificateManagerML::creator(void *pointerToAppObject)
{
    CertificateManager * lCM =  new CertificateManager;
    lCM->mApp = reinterpret_cast<App *>(pointerToAppObject);
    return lCM;
}

bool CertificateManagerML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)

    CertificateManager *    lCM                 = reinterpret_cast<CertificateManager *>(pointerToThis);

    if( lCM ) {
        lCM->loadConfigurationValues();
        return lCM->generateCertificates();
    }

    return false;
}

bool CertificateManagerML::startInOwnThread()
{ return false; }

bool CertificateManagerML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)

    CertificateManager *    lCM     = reinterpret_cast<CertificateManager *>(pointerToThis);

    if( lCM ) {
        lCM->loadConfigurationValues();
        if( lCM->loadCertificates() ) {
            lCM->mApp->setCertificates(lCM);
            return true;
        }
    }

    return false;
}

bool CertificateManager::_saveFile(const QByteArray &iData, const QString &iFilename)
{
    QFile lFile{iFilename};

    if( lFile.open(QIODevice::WriteOnly) )
        return lFile.write(iData) >= iData.size();

    return false;
}

QByteArray CertificateManager::_loadFile(const QString &iFilename)
{
    QFile   lFile{iFilename};

    if( lFile.open(QIODevice::ReadOnly))
        return lFile.readAll();
    return QByteArray();
}

Certificate *CertificateManager::_loadCertificateFilename(const QString &iFilename)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new Certificate{lData};
        }catch(...){
        }
    }

    return nullptr;
}

EncryptionKey *CertificateManager::_loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new EncryptionKey{lData,iIsPrivateKey};
        }catch(...){
        }
    }

    return nullptr;
}

bool CertificateManager::_loadCertificatePEMOrFile(QByteArray &ioPEM, const QString iFilename, Certificate **oCertificate)
{
    if( ioPEM.isEmpty() ) {
        if( iFilename.isEmpty() )
            return false;
        *oCertificate = CertificateManager::_loadCertificateFilename(iFilename);
        if( *oCertificate == nullptr )
            return false;
        ioPEM = (*oCertificate)->toPEM();
        return true;
    }else{
        try {
            *oCertificate = new Certificate{ioPEM};
            return true;
        }catch(...){
            return false;
        }
    }
    return false;
}

bool CertificateManager::_loadEncryptionKeyPEMOrFile(QByteArray &ioPEM, const QString iFilename, EncryptionKey **oKey, bool iIsPrivateKey)
{
    if( ioPEM.isEmpty() ) {
        if( iFilename.isEmpty() )
            return false;
        *oKey = CertificateManager::_loadEncryptionKeyFilename(iFilename,iIsPrivateKey);
        if( *oKey == nullptr )
            return false;
        if( iIsPrivateKey )
            ioPEM = (*oKey)->privateToPEM();
        else
            ioPEM = (*oKey)->publicToPEM();
        return true;
    }else{
        try {
            *oKey = new EncryptionKey{ioPEM,iIsPrivateKey};
            return true;
        }catch(...){
            return false;
        }
    }
    return false;
}

CertificateManager::~CertificateManager()
{
    if( CACertificate )         delete CACertificate;
    if( CAPrivateKey )          delete CAPrivateKey;
    if( ServerCertificate )     delete ServerCertificate;
    if( ServerPrivateKey )      delete ServerPrivateKey;
}

void CertificateManager::loadConfigurationValues()
{
    if( ! mApp ) return;

    auto lConf = mApp->configuration();
    CACertificateFilename       = lConf->toString(QStringLiteral("CACertificateFilename"));
    CAPrivateKeyFilename        = lConf->toString(QStringLiteral("CAPrivateKeyFilename"));
    ServerCertificateFilename   = lConf->toString(QStringLiteral("ServerCertificateFilename"));
    ServerPrivateKeyFilename    = lConf->toString(QStringLiteral("ServerPrivateKeyFilename"));
    CACertificatePEM            = lConf->toByteArray(QStringLiteral("CACertificatePEM"));
    CAPrivateKeyPEM             = lConf->toByteArray(QStringLiteral("CAPrivateKeyPEM"));
    ServerCertificatePEM        = lConf->toByteArray(QStringLiteral("ServerCertificatePEM"));
    ServerPrivateKeyPEM         = lConf->toByteArray(QStringLiteral("ServerPrivateKeyPEM"));
}

bool CertificateManager::loadCertificates()
{
    // Try to load everything even if something fails
    bool    lCACertLoaded           = CertificateManager::_loadCertificatePEMOrFile( CACertificatePEM, CACertificateFilename, &(CACertificate) );
    bool    lServerCertLoaded       = CertificateManager::_loadCertificatePEMOrFile( ServerCertificatePEM, ServerCertificateFilename, &(ServerCertificate) );
    bool    lServerPrivKeyLoaded    = CertificateManager::_loadEncryptionKeyPEMOrFile( ServerPrivateKeyPEM, ServerPrivateKeyFilename, &(ServerPrivateKey) );

    // Do optional load but ignore the result
    CertificateManager::_loadEncryptionKeyPEMOrFile( CAPrivateKeyPEM, CAPrivateKeyFilename, &(CAPrivateKey) );

    return (lCACertLoaded && lServerCertLoaded && lServerPrivKeyLoaded);
}

bool CertificateManager::generateCertificates()
{
    Certificate::initializeLibrary();

    loadCertificates(); // Ignore the results

    auto lConfig                    = mApp->configuration();
    QString     lProductFQDN        = lConfig->toString(QStringLiteral("ProductFQDN"));
    QStringList lServerNames        = lConfig->toStringList(QStringLiteral("ServerName"));
    QStringList lServerAddresses    = lConfig->toStringList(QStringLiteral("ServerAddress"));
    bool        lCAFilesSaved       = true;
    bool        lServerFilesSaved   = true;

    // Generate a CA Certificate and Private Key IF:
    // 1. The CA Certificate is NULL **OR** #2
    // 2. The CA Private Key is NULL **AND** the Server Certificate and/or Private Key is NULL

    if( CACertificate == nullptr || (CAPrivateKey == nullptr && (ServerCertificate == nullptr || ServerPrivateKey == nullptr)) ) {
        CACertificate = new Certificate;
        CACertificate->setCADefaults();
        CACertificate->setIssuerName(QStringList() << QStringLiteral("CN=%1").arg(lProductFQDN));
        CACertificate->setSubjectNames(QStringList() << QStringLiteral("CN=%1").arg(lProductFQDN));
        CACertificate->selfSign();

        // Serialize and create the secondary objects
        CACertificatePEM    = CACertificate->toPEM();
        CAPrivateKeyPEM     = CACertificate->encryptionKey()->privateToPEM();
        CAPrivateKey        = new EncryptionKey{CAPrivateKeyPEM};

        lConfig->setValue(QStringLiteral("CACertificatePEM"), CACertificatePEM);

        if( ! CACertificateFilename.isEmpty() && ! _saveFile(CACertificatePEM,CACertificateFilename) )
            lCAFilesSaved = false;

        if( ! CAPrivateKeyFilename.isEmpty() && ! _saveFile(CAPrivateKeyPEM,CAPrivateKeyFilename) )
            lCAFilesSaved = false;
    }

    // Generate a Server Certificate and Private Key IF:
    // 1. The CA Certificate and CA Private Key is NOT NULL **AND** #2
    // 2. The Server Certificate **OR** the Server Private Key is NULL

    if( CACertificate != nullptr && CAPrivateKey != nullptr ) {
        if( ServerCertificate == nullptr || ServerPrivateKey == nullptr ) {
            ServerCertificate = new Certificate;
            ServerCertificate->setServerDefaults();
            ServerCertificate->setIssuerName(QStringList() << QStringLiteral("CN=%1").arg(lProductFQDN));
            ServerCertificate->setSerialNumber(QDateTime::currentSecsSinceEpoch());

            for( QString lSN : lServerNames )
            { ServerCertificate->addServerName(lSN); }

            for( QString lSN : lServerAddresses )
            { ServerCertificate->addServerName(lSN); }

            ServerCertificate->sign(CAPrivateKey);

            // Serialize and create the secondary objects
            ServerCertificatePEM    = ServerCertificate->toPEM();
            ServerPrivateKeyPEM     = ServerCertificate->encryptionKey()->privateToPEM();

            lConfig->setValue(QStringLiteral("ServerCertificatePEM"), ServerCertificatePEM);
            lConfig->setValue(QStringLiteral("ServerPrivateKeyPEM"), ServerPrivateKeyPEM);

            if( ! ServerCertificateFilename.isEmpty() && ! _saveFile(ServerCertificatePEM,ServerCertificateFilename) )
                lServerFilesSaved = false;

            if( ! ServerPrivateKeyFilename.isEmpty() && ! _saveFile(ServerPrivateKeyPEM,ServerPrivateKeyFilename) )
                lServerFilesSaved = false;
        }
    }

    return (lServerFilesSaved && lCAFilesSaved && ! CACertificatePEM.isEmpty() && ! ServerCertificatePEM.isEmpty() && ! ServerPrivateKeyPEM.isEmpty() );
}
