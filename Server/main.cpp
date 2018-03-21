#include "main.h"

App * gApp = nullptr;

//#include "init.h"

//#include <QString>
//#include <QVariant>
//#include <QVariantMap>
//#include <QJsonDocument>
//#include <QFile>
#include <QCoreApplication>

//#include <stdio.h>

//quint16             gRESTPort           = 0;
//QString             gSettingsFile       = QStringLiteral(kSettingsFile);
//QString             gCACertFile         = QStringLiteral("");
//QString             gPrivateKeyFile     = QStringLiteral("");
//QString             gCertificateFile    = QStringLiteral("");
//QString             gLogsPath           = QStringLiteral("");
//QString             gRecordsPath        = QStringLiteral("");

//QThread *           gDownloaderThread   = nullptr;
//Downloader *        gDownloader         = nullptr;

//QThread *           gLogsManagerThread  = nullptr;
//LogsManager *       gLogsManager        = nullptr;

//RecordsManager *    gRecordsManager     = nullptr;

int main(int argc, char *argv[])
{
//    qInstallMessageHandler(LogsManager::messageHandler);
    QCoreApplication lApp(argc, argv);

//    ////////////////////////////////////////
//    /// Parse command line options.
//    ///
//    bool lInitConfig = false;

//    for( int lIndex = 1; lIndex < argc; lIndex++ ) {
//        QString lArgv = argv[lIndex];

//        // To override the default settings file
//        if( lArgv == "-config" && lIndex+1 < argc ) {
//            lIndex++;
//            gSettingsFile = argv[lIndex];
//            continue;
//        }

//        // To override the CA file
//        if( lArgv == "-cafile" && lIndex+1 < argc ) {
//            lIndex++;
//            gCACertFile = argv[lIndex];
//            continue;
//        }

//        // To override the Certificate file
//        if( lArgv == "-certfile" && lIndex+1 < argc ) {
//            lIndex++;
//            gCertificateFile = argv[lIndex];
//            continue;
//        }

//        // To override the Private Key file
//        if( lArgv == "-privfile" && lIndex+1 < argc ) {
//            lIndex++;
//            gPrivateKeyFile = argv[lIndex];
//            continue;
//        }

//        // To create a new files: settings, ca, cert, private key, etc
//        if( lArgv == "-initConfig" ) {
//            lInitConfig = true;
//            continue;
//        }
//    }
//    ///
//    ///
//    ////////////////////////////////////////


//    ////////////////////////////////////////
//    /// Do init config?
//    ///
//    if( lInitConfig ) {
//        initConfigFile(gSettingsFile,gCACertFile,gCertificateFile,gPrivateKeyFile);
//        return 0;
//    }
//    ///
//    ///
//    ////////////////////////////////////////


//    ////////////////////////////////////////
//    /// Load settings.
//    ///
//    QFile   lSettingsFile{gSettingsFile};
//    if( lSettingsFile.open(QIODevice::ReadOnly) ) {
//        QByteArray lSettingsData = lSettingsFile.readAll();
//        if( ! lSettingsData.isEmpty() ) {
//            QJsonParseError lParseError;
//            QJsonDocument   lSettingsJson   = QJsonDocument::fromJson( lSettingsData, &lParseError );
//            if( lParseError.error == QJsonParseError::NoError ) {
//                QVariantMap lSettings = lSettingsJson.toVariant().toMap();
//                if( ! lSettings.isEmpty() ) {
//                    if( lSettings.contains(QStringLiteral(kRESTPortKey)) ) {
//                        gRESTPort = static_cast<quint16>(lSettings[QStringLiteral(kRESTPortKey)].toUInt() & 0xFFFF);
//                    }

//                    if( lSettings.contains(QStringLiteral(kCACertFileKey)) ) {
//                        gCACertFile = lSettings[QStringLiteral(kCACertFileKey)].toString();
//                    }

//                    if( lSettings.contains(QStringLiteral(kPrivateKeyFile)) ) {
//                        gPrivateKeyFile = lSettings[QStringLiteral(kPrivateKeyFile)].toString();
//                    }

//                    if( lSettings.contains(QStringLiteral(kCertificateFileKey)) ) {
//                        gCertificateFile = lSettings[QStringLiteral(kCertificateFileKey)].toString();
//                    }

//                    if( lSettings.contains(QStringLiteral(kLogsPath)) ) {
//                        gLogsPath = lSettings[QStringLiteral(kLogsPath)].toString();
//                    }

//                    if( lSettings.contains(QStringLiteral(kRecordsPath)) ) {
//                        gRecordsPath = lSettings[QStringLiteral(kRecordsPath)].toString();
//                    }
//                }
//            }
//        }
//    }else{
//        qFatal("Unable to open the settings file '%s'.", kSettingsFile);
//        return 1;
//    }
//    ///
//    /// Finished loading settings.
//    ////////////////////////////////////////


//    ////////////////////////////////////////
//    /// Validate settings
//    ///
//    if( gLogsPath.isEmpty() )
//    { qFatal("No logs path was specified!"); return 2; }

//    gLogsManagerThread = new QThread{&lApp};
//    gLogsManager = new LogsManager{gLogsPath};
//    gLogsManager->moveToThread(gLogsManagerThread);
//    QCoreApplication::connect(gLogsManagerThread, &QThread::started, gLogsManager, &LogsManager::threadStarted);
//    gLogsManagerThread->start();

//    if( gRecordsPath.isEmpty() )
//    { qFatal("No records path was specified!"); return 2; }

//    if( gCertificateFile.isEmpty() )
//    { qFatal("No certificate file was specified!"); return 2; }

//    if( gPrivateKeyFile.isEmpty() )
//    { qFatal("No private key file was specified!"); return 2; }

//    if( gCACertFile.isEmpty() )
//    { qFatal("No CA certificate file was specified!"); return 2; }

//    if( gRESTPort < 1 )
//    { qFatal("No valid REST port was specified!"); return 2; }

//    ///
//    /// Finished validating settings
//    ////////////////////////////////////////

//    ////////////////////////////////////////
//    /// Remove old logs and records.
//    ///
//#warning TODO
//    ///
//    /// Finished removing old logs and
//    /// recods.
//    ////////////////////////////////////////


//    ////////////////////////////////////////
//    /// Create Downloader object and start
//    /// it in another thread.
//    ///
//    gDownloaderThread = new QThread(&lApp);
//    gDownloader = new Downloader;
//    gDownloader->moveToThread(gDownloaderThread);
//    QCoreApplication::connect(gDownloaderThread,&QThread::started,gDownloader,&Downloader::threadStarted);
//    gDownloaderThread->start();
//    ///
//    /// Finished setting up Downloader.
//    ////////////////////////////////////////


//    ////////////////////////////////////////
//    /// Create HTTPS server and start it.
//    ///
//#warning TODO
//    ///
//    /// Finished setting up HTTPS server.
//    ////////////////////////////////////////

    gApp = new App{argc,argv};
    gApp->parseCommandLine();
    if( gApp->shouldDoInit() ) {
        if( gApp->doInit() ) return 0;
        return 1;
    }
    gApp->loadSettings();
    gApp->parseCommandLine();
    gApp->loadCryptoFiles();
    gApp->start();

    return lApp.exec();
}
