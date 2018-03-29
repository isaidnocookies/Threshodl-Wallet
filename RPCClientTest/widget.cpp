#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QUrl>
#include <QFile>
#include <QSslConfiguration>
#include <QSslCertificate>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->statusLabel->setVisible(false);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_connectButton_pressed()
{
    ui->statusLabel->setVisible(true);

    QString lUrlString = QStringLiteral("wss://%1:%2").arg(ui->hostnameLineEdit->text()).arg(ui->portLineEdit->text());
    RPCConnection * lRPC = new RPCConnection;

    QFile lFile{QStringLiteral("/home/josephcohen/SourceCache/wallet/build-Server-Desktop_Qt_5_10_1_GCC_64bit-Debug/cacert.pem")};
    if( lFile.open(QIODevice::ReadOnly) ) {
        QSslConfiguration lSslConf = QSslConfiguration::defaultConfiguration();
        lSslConf.setCaCertificates(lSslConf.caCertificates() << QSslCertificate{lFile.readAll(),QSsl::Pem});

        qDebug() << lSslConf.caCertificates().size();

        lFile.close();
        lRPC->setSslConfiguration(lSslConf);
        QSslConfiguration::setDefaultConfiguration(lSslConf);
    }

    connect( lRPC, &RPCConnection::connected, this, &Widget::rpcOpenned );
    connect( lRPC, &RPCConnection::socketError, this, &Widget::rpcSocketError );
    connect( lRPC, &RPCConnection::sslErrors, this, &Widget::rpcSslErrors );

    lRPC->open(QUrl::fromUserInput(lUrlString));
}

void Widget::rpcOpenned()
{
    ui->statusLabel->setText(QStringLiteral("Connected!"));
}

void Widget::rpcSslErrors(const QList<QSslError> iErrors)
{
    qDebug() << "Received" << iErrors.size() << "Ssl errors:";
    int lIndex = 0;
    for( auto lError : iErrors ) {
        qDebug() << "[" << lIndex++ << "]" << lError.errorString();
    }
}

void Widget::rpcSocketError(QAbstractSocket::SocketError iError)
{
    ui->statusLabel->setText(QStringLiteral("FAILED!"));
    qDebug() << "Socket error" << iError;
}
