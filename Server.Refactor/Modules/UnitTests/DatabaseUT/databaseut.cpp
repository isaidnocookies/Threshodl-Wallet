#include "databaseut.h"
#include "modulelinker.h"
#include "app.h"
#include "../../V1/Database/database.h"

#include "certificate.h"

#include <QDebug>
#include <QString>
#include <QStringList>

#ifdef UNIT_TEST_ON
static DatabaseUT   _gRegisterModuleLinker;
#else
static void *       _gRegisterModuleLinker = nullptr; // To prevent compiler errors
#endif

DatabaseUT::DatabaseUT()
{
    QStringList lDependencies = QStringList() << QStringLiteral("LogsManager-1") << QStringLiteral("CertificateManager-1") << QStringLiteral("Database-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("DatabaseUnitTest-1"),lDependencies,DatabaseUT::creator,DatabaseUT::doInit,DatabaseUT::start,DatabaseUT::startInOwnThread);
}

void *DatabaseUT::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return &_gRegisterModuleLinker;
}

bool DatabaseUT::startInOwnThread()
{ return false; }

bool DatabaseUT::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool DatabaseUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)

    bool            lResult = true;

    App *           lApp    = reinterpret_cast<App *>(pointerToAppObject);
    Database *      lDB     = reinterpret_cast<Database *>(lApp->getModuleObject(QStringLiteral("Database-1")));

    auto lConfig = lApp->configuration();

    // Skip DatabaseUT if told too
    if( lConfig->contains(QStringLiteral("DatabaseUT")) ) {
        QString lValue = lConfig->toString(QStringLiteral("DatabaseUT"));
        lConfig->removeValue(QStringLiteral("DatabaseUT"));
        if( lValue == QStringLiteral("Skip") )
            return true;
    }

    Certificate     lTestUser1;
    QString         lTimeStamp      = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString         lTestUser1Name  = QStringLiteral("##TestUser1@%1").arg(lTimeStamp);
//    QByteArray      lPrivKey1       = lTestUser1.encryptionKey()->privateToPEM();
    QByteArray      lPubKey1        = lTestUser1.encryptionKey()->publicToPEM();

    qDebug() << "Performing Test 1, Creating a test user account, checking it's existance, validating it and then deleting it:";
    if( lResult && lDB->addressCreate( lTestUser1Name, lPubKey1 ) ) {
        if( lDB->addressExists( lTestUser1Name) ) {
            if( lDB->addressValidate( lTestUser1Name, lPubKey1 ) ) {
                if( lDB->addressDelete( lTestUser1Name ) ) {
                    qDebug() << "    Successful";
                }else{
                    qWarning() << "    Failed";
                    lResult = false;
                }
            }else{
                qWarning() << "    Failed";
                lResult = false;
            }
        }else{
            qWarning() << "    Failed";
            lResult = false;
        }
    }else{
        qWarning() << "    Failed";
        lResult = false;
    }

    if( lResult ) {
        qDebug() << "Performing Test 2, Creating a test user account and then attempting to register that same account again (which should fail, a success here means it failed as expected - this test cleans up after itself on success):";
        if( lDB->addressCreate(lTestUser1Name,lPubKey1) ) {
            if( ! lDB->addressCreate(lTestUser1Name,lPubKey1) ) {
                if( lDB->addressDelete(lTestUser1Name) ) {
                    qDebug() << "    Successful";
                }else{
                    qWarning() << "    Failed";
                    lResult = false;
                }
            }else{
                qWarning() << "    Failed";
                lResult = false;
            }
        }else{
            qWarning() << "    Failed";
            lResult = false;
        }
    }


    return lResult;
}
