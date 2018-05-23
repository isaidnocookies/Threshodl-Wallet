#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QZXing.h>

#include "platforminformation.h"
#include "useraccount.h"
#include "marketvalueupdater.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);


    qmlRegisterType<PlatformInformation>("com.threeb.threshodl.PlatformInformation", 1, 0, "PlatformInformation");
    qmlRegisterType<UserAccount>("com.threeb.threshodl.UserAccount", 1, 0, "UserAccount");
    qmlRegisterType<MarketValueUpdater>("com.threeb.threshodl.MarketValueUpdater", 1, 0, "MarketValueUpdater");

    QQmlApplicationEngine engine;

    QZXing::registerQMLTypes();
    QZXing::registerQMLImageProvider(engine);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
