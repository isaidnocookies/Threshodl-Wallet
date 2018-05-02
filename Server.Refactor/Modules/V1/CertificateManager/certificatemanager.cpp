#include "certificatemanager.h"
#include "app.h"

#include <QDebug>
#include <QFile>

static CertificateManagerML _gRegisterModuleLinker;

QByteArray CertificateManager::_loadFile(const QString &iFilename)
{
    QFile   lFile{iFilename};

    if( lFile.open(QIODevice::ReadOnly))
        return lFile.readAll();
    return QByteArray();
}

CertificateManager::Certificate *CertificateManager::_loadCertificateFilename(const QString &iFilename)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new Certificate{lData};
        }catch(...){
            return nullptr;
        }
    }
}

EncryptionKey *CertificateManager::_loadEncryptionKeyFilename(const QString &iFilename, bool iIsPrivateKey)
{
    QByteArray lData = _loadFile(iFilename);
    if( ! lData.isEmpty() ) {
        try {
            return new EncryptionKey{lData,iIsPrivateKey};
        }catch(...){
            return nullptr;
        }
    }

    return nullptr;
}

bool CertificateManager::_loadCertificatePEMOrFile(const QByteArray iPEM, const QString iFilename, Certificate **oCertificate)
{
    if( iPEM.isEmpty() ) {
        if( iFilename.isEmpty() )
            return false;
        *oCertificate = CertificateManager::_loadCertificateFilename(iFilename);
        if( *oCertificate == nullptr )
            return false;
        return true;
    }else{
        try {
            *oCertificate = new Certificate{iPEM};
            return true;
        }catch(...){
            return false;
        }
    }
    return false;
}

bool CertificateManager::_loadEncryptionKeyPEMOrFile(const QByteArray iPEM, const QString iFilename, EncryptionKey **oKey, bool iIsPrivateKey)
{
    if( iPEM.isEmpty() ) {
        if( iFilename.isEmpty() )
            return false;
        *oKey = CertificateManager::_loadEncryptionKeyFilename(iFilename);
        if( *oKey == nullptr )
            return false;
        return true;
    }else{
        try {
            *oKey = new EncryptionKey{iPEM,iIsPrivateKey};
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

void CertificateManager::loadConfigurationValuesFromApp(App *iApp)
{
    if( ! iApp ) return;

    auto lConf = iApp->configuration();
    CACertificateFilename       = lConf->toString(QStringLiteral("CACertificateFilename"));
    CAPrivateKeyFilename        = lConf->toString(QStringLiteral("CAPrivateKeyFilename"));
    ServerCertificateFilename   = lConf->toString(QStringLiteral("ServerCertificateFilename"));
    ServerPrivateKeyFilename    = lConf->toString(QStringLiteral("ServerPrivateKeyFilename"));
    CACertificatePEM            = lConf->toByteArray(QStringLiteral("CACertificatePEM"));
    CAPrivateKeyPEM             = lConf->toByteArray(QStringLiteral("CAPrivateKeyPEM"));
    ServerCertificatePEM        = lConf->toByteArray(QStringLiteral("ServerCertificatePEM"));
    ServerPrivateKeyPEM         = lConf->toByteArray(QStringLiteral("ServerPrivateKeyPEM"));
}

CertificateManagerML::CertificateManagerML()
{ }

void *CertificateManagerML::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return new CertificateManager;
}

bool CertificateManagerML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    App *                   lApp                = reinterpret_cast<App *>(pointerToAppObject);
    CertificateManager *    lCM                 = reinterpret_cast<CertificateManager *>(pointerToThis);
    QStringList             lServerNames;
    QStringList             lServerAddresses;

    if( lApp && lCM ) {
        lCM->loadConfigurationValuesFromApp(lApp);

        auto lConfig        = lApp->configuration();
        lServerNames        = lConfig->toStringList(QStringLiteral("ServerName"));
        lServerAddresses    = lConfig->toStringList(QStringLiteral("ServerAddress"));

        // Try to load what we can
        CertificateManager::_loadCertificatePEMOrFile( lCM->CACertificatePEM, lCM->CACertificateFilename, &(lCM->CACertificate) );
        CertificateManager::_loadCertificatePEMOrFile( lCM->ServerCertificatePEM, lCM->ServerCertificateFilename, &(lCM->ServerCertificate) );
        CertificateManager::_loadEncryptionKeyPEMOrFile( lCM->ServerPrivateKeyPEM, lCM->ServerPrivateKeyFilename, &(lCM->ServerPrivateKey) );
        CertificateManager::_loadEncryptionKeyPEMOrFile( lCM->CAPrivateKeyPEM, lCM->CACertificateFilename, &(lCM->CAPrivateKey) );

        // Now generate the rest
        if( ! lCM->CACertificate || ! lCM->CAPrivateKey ) {
            // We can't generate and sign a server cert, before we go down this rabbit hole lets see if we need to generate a server cert.
            if( ! lCM->ServerCertificate || ! lCM->ServerPrivateKey ) {
                // We NEED a CA Cert and a CA Private Key so lets make them

            }
        }
    }

    return false;
}

bool CertificateManagerML::startInOwnThread()
{ return false; }

bool CertificateManagerML::start(void *pointerToThis, void *pointerToAppObject)
{
    App *                   lApp    = reinterpret_cast<App *>(pointerToAppObject);
    CertificateManager *    lCM     = reinterpret_cast<CertificateManager *>(pointerToThis);

    if( lApp && lCM ) {
        lCM->loadConfigurationValuesFromApp(lApp);

        if(
            CertificateManager::_loadCertificatePEMOrFile( lCM->CACertificatePEM, lCM->CACertificateFilename, &(lCM->CACertificate) )               &&
            CertificateManager::_loadCertificatePEMOrFile( lCM->ServerCertificatePEM, lCM->ServerCertificateFilename, &(lCM->ServerCertificate) )   &&
            CertificateManager::_loadEncryptionKeyPEMOrFile( lCM->ServerPrivateKeyPEM, lCM->ServerPrivateKeyFilename, &(lCM->ServerPrivateKey) )
        ) {
            // Do optional load
            CertificateManager::_loadEncryptionKeyPEMOrFile( lCM->CAPrivateKeyPEM, lCM->CACertificateFilename, &(lCM->CAPrivateKey) );
            return true;
        }

    }

    return false;
}
