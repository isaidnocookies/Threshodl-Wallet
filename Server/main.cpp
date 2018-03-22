#include "main.h"

App * gApp = nullptr;

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication lApp(argc, argv);
    lApp.setApplicationName(QStringLiteral("Threshodl"));
    lApp.setOrganizationName(QStringLiteral("Threshodl"));
    lApp.setOrganizationDomain(QStringLiteral("com.threshodl"));

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
