#include "feeestimatorut.h"
#include "../../Interface/FeeEstimatorInterface/feeestimatorinterface.h"
#include "app.h"
#include "modulelinker.h"

#include <QElapsedTimer>
#include <QSignalSpy>
#include <QDebug>

#ifdef UNIT_TEST_ON
static FeeEstimatorUT   _gRegisterModuleLinker;
#else
static void *           _gRegisterModuleLinker = nullptr; // To prevent compiler errors
#endif

FeeEstimatorUT::FeeEstimatorUT()
{
    QStringList lDependencies = QStringList() << QStringLiteral("Downloader-1") << QStringLiteral("RecordsManager-1") << QStringLiteral("FeeEstimator-1");
    ModuleLinker::registerModuleWithDependencies(QStringLiteral("FeeEstimatorUnitTest-1"),lDependencies,FeeEstimatorUT::creator,FeeEstimatorUT::doInit,FeeEstimatorUT::start,FeeEstimatorUT::startInOwnThread);
}

void *FeeEstimatorUT::creator(void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    return &_gRegisterModuleLinker;
}

bool FeeEstimatorUT::startInOwnThread()
{ return false; }

bool FeeEstimatorUT::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)
    Q_UNUSED(pointerToAppObject)
    return true;
}

bool FeeEstimatorUT::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis)

    App *                   lApp    = reinterpret_cast<App *>(pointerToAppObject);
    FeeEstimatorInterface * lFE     = reinterpret_cast<FeeEstimatorInterface *>(lApp->getModuleObject(QStringLiteral("FeeEstimator-1")));

    if( lApp && lFE ) {

        auto lConfig = lApp->configuration();

        // Skip FeeEstimatorUT if told too
        if( lConfig->contains(QStringLiteral("FeeEstimatorUT")) ) {
            QString lValue = lConfig->toString(QStringLiteral("FeeEstimatorUT"));
            lConfig->removeValue(QStringLiteral("FeeEstimatorUT"));
            if( lValue == QStringLiteral("Skip") )
                return true;
        }

        // Build list of Cryptos and Types
        QList< QString >    lCryptosAndTypes;
        for( QVariant lE : lConfig->toVariantList(QStringLiteral("FeeEstimationSources")) )
        {
            QVariantMap lEntry      = lE.toMap();
            QString     lCrypto     = lEntry["crypto"].toString();
            QString     lType;

            if( lCrypto == QStringLiteral("BTC") )
                lType = lEntry["chain"].toString();

            lCryptosAndTypes << QStringLiteral("%1.%2").arg(lCrypto).arg(lType);
        }

        QSignalSpy      lSignalSpy(lFE, SIGNAL(recordFee(QString,QString,QString)));

        // Need to download a list of crypto currencies and then check to make sure we get we have downloaded each one
        // Before saying the module is good

        qDebug() << "Performing FeeEstimator Unit test...";
        QElapsedTimer   lTimer;
        lTimer.start();
        while( ! lCryptosAndTypes.isEmpty() ) {
            if( lTimer.elapsed() > 120000 ) {
                qCritical() << "Timed out waiting for all of the fee estimations to download!";
                return false;
            }

            qDebug() << "Waiting for a download event to occur.";
            if( ! lSignalSpy.wait(30000) ) {
                qWarning() << "Timed out waiting for a fee estimation download!";
            }

            while( ! lSignalSpy.isEmpty() ) {
                QList<QVariant> lArguments  = lSignalSpy.takeFirst();
                QString         lEmitCrypto = lArguments.at(0).toString();
                QString         lEmitType   = lArguments.at(1).toString();
                QString         lEmitFee    = lArguments.at(2).toString();

                qDebug() << "FeeEstimator recorded fee" << lEmitFee << "for type" << lEmitType << "for crypto currency" << lEmitCrypto;
                lCryptosAndTypes.removeAll(QStringLiteral("%1.%2").arg(lEmitCrypto).arg(lEmitType));
            }
        }


        return true;
    }

    return false;
}
