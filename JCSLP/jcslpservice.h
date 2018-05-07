#ifndef JCSLPSERVICE_H
#define JCSLPSERVICE_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QHostAddress>

namespace JCSLP
{
namespace SERVICE
{
namespace V1
{

bool processServiceString(const QString iService, QString &oNameOfService, QString &oLocation, QString &oProtocol, QString &oNetworkAddress, quint16 &oPort);

QString createService(const QString iNameOfService, const QString iLocation, const QString iProtocol, quint16 iPort, const QString iNetworkAddress);
QStringList createServiceList(const QString iNameOfService, const QString iLocation, const QStringList iProtocols, quint16 iPort, const QStringList iNetworkAddresses);
QStringList createServiceList(const QString iNameOfService, const QString iLocation, quint16 iPort, const QStringList iProtocols = (QStringList() << QStringLiteral("TCP")), const QList<QHostAddress> iNetworkAddresses = (QList<QHostAddress>() << QHostAddress::Any));

}
}
}

#endif // JCSLPSERVICE_H
