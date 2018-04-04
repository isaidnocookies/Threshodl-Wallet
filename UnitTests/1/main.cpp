#include "test.h"

#include <QFile>
#include <QDebug>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication lApp(argc, argv);

//    RPCConnection * lConnection = RPCConnection::open(QUrl(QStringLiteral("wss://127.0.0.1:4331")));

    RPCConnection * lRPC = new RPCConnection;

    QFile lFile{QStringLiteral("/tmp/ca.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        QSslConfiguration lSslConf = QSslConfiguration::defaultConfiguration();
        lSslConf.setCaCertificates(lSslConf.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        lFile.close();
        lRPC->setSslConfiguration(lSslConf);
    }

    new Test{lRPC};

//    if( lConnection ) {
//        // /tmp/ca.pem
//    }else{
//        qDebug() << "Failed to connect to server";
//    }

//    return 1;

    return lApp.exec();
}
