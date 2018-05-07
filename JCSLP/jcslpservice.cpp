#include "jcslpservice.h"

#include <QNetworkInterface>

QString JCSLP::SERVICE::V1::createService(const QString iNameOfService, const QString iLocation, const QString iProtocol, quint16 iPort, const QString iNetworkAddress)
{ return QStringLiteral("%1\n%2\n%3\n%4\n%5").arg(iNameOfService).arg(iLocation).arg(iProtocol).arg(iPort).arg(iNetworkAddress); }

QStringList JCSLP::SERVICE::V1::createServiceList(const QString iNameOfService, const QString iLocation, const QStringList iProtocols, quint16 iPort, const QStringList iNetworkAddresses)
{
    QStringList lResult;
    for( QString lProtocol : iProtocols )
    {
        for( QString lAddress : iNetworkAddresses )
        { lResult << JCSLP::SERVICE::V1::createService(iNameOfService,iLocation,lProtocol,iPort,lAddress); }
    }
    return lResult;
}

QStringList JCSLP::SERVICE::V1::createServiceList(const QString iNameOfService, const QString iLocation, quint16 iPort, const QStringList iProtocols, const QList<QHostAddress> iNetworkAddresses)
{
    QStringList             lAddresses;
    QList<QHostAddress>     lHostAddresses  = iNetworkAddresses;

    lHostAddresses.removeAll(QHostAddress::Any);
    lHostAddresses.removeAll(QHostAddress::AnyIPv4);
    lHostAddresses.removeAll(QHostAddress::AnyIPv6);

    if( iNetworkAddresses.contains(QHostAddress::Any) || iNetworkAddresses.contains(QHostAddress::AnyIPv4) || iNetworkAddresses.contains(QHostAddress::AnyIPv6) ) {
        for( QHostAddress lHA : QNetworkInterface::allAddresses() )
        {
            if( lHostAddresses.contains(lHA) ) continue;
            lHostAddresses.append(lHA);
        }
    }

    for( QHostAddress lHA : lHostAddresses )
    {
        QString lHAString = lHA.toString();
        if( lAddresses.contains(lHAString) ) continue;
        lAddresses << lHA.toString();
    }

    return JCSLP::SERVICE::V1::createServiceList(iNameOfService,iLocation,iProtocols,iPort,lAddresses);
}

bool JCSLP::SERVICE::V1::processServiceString(const QString iService, QString &oNameOfService, QString &oLocation, QString &oProtocol, QString &oNetworkAddress, quint16 &oPort)
{
    QStringList lList = iService.split(QStringLiteral("\n"));

    if( lList.size() == 5 ) {
        oNameOfService  = lList[0];
        oLocation       = lList[1];
        oProtocol       = lList[2];
        oPort           = lList[3].toUInt();
        oNetworkAddress = lList[4];
        return true;
    }

    return false;
}
