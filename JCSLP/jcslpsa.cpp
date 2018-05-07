#include "jcslpsa.h"
#include "jcslppdu.h"
#include "jcslpservice.h"

#include <QHostInfo>
#include <QNetworkInterface>
#include <QThread>
#include <QtEndian>
#include <QNetworkDatagram>

#define     kJCSLPPort              1030

inline static bool __filterMatchString(const QString iFirst, const QString iSecond)
{
    if(
        iFirst              == QStringLiteral("*")      ||
        iSecond             == QStringLiteral("*")      ||
        iFirst.toLower()    ==  iSecond.toLower()
    ) {
        return true;
    }

    return false;
}

inline static bool __filterMatchPort(quint16 iFirst, quint16 iSecond)
{
    if(
        iFirst  == 0        ||
        iSecond == 0        ||
        iFirst  == iSecond
    ) {
        return true;
    }

    return false;
}

bool JCSLP::SA::_inDAMode() const
{
    QReadLocker l{&mDAListLock};
    return mInDAMode;
}

void JCSLP::SA::_sendSAPDU(QUdpSocket *iSocket)
{
    if( mSAPDU.isEmpty() && ! mServices.isEmpty() )
        mSAPDU = JCSLP::PDU::V1::createSAAnnounce(mServices);

    if( mSAPDU.isEmpty() )
        return;

    if( iSocket ) {
        iSocket->writeDatagram( mSAPDU, QHostAddress::Broadcast, kJCSLPPort );
    }else{
        for( QUdpSocket * lSocket : mSockets )
        { lSocket->writeDatagram( mSAPDU, QHostAddress::Broadcast, kJCSLPPort ); }
    }
}

void JCSLP::SA::_sendDAListPDU(QNetworkDatagram &iReceivedDatagram, QUdpSocket * iSocket)
{
    QStringList     lDAList;

    mDAListLock.lockForRead();
    if( mDAStringCacheList.isEmpty() ) {
        // Need to reconstruct it
        mDAListLock.unlock();

        mDAListLock.lockForWrite();
        // Double check
        if( mDAStringCacheList.isEmpty() ) {
            // It is still empty, reconstruct it
            for( QHostAddress lHA : mDAList )
            {
                QString lHAStr = lHA.toString();
                if( lDAList.contains(lHAStr) ) continue;
                lDAList << lHAStr;
            }
        }
    }else{
        lDAList = mDAStringCacheList;
    }
    mDAListLock.unlock();

    QByteArray  lPDU = JCSLP::PDU::V1::createDAAnnounce(lDAList);
    if( ! lPDU.isEmpty() ) {
        if( iSocket  != nullptr ) {
            iSocket->writeDatagram(iReceivedDatagram.makeReply(lPDU));
        }else{
            QUdpSocket().writeDatagram(iReceivedDatagram.makeReply(lPDU));
        }
    }
}

void JCSLP::SA::_processUARequest(QNetworkDatagram &iReceivedDatagram, QUdpSocket *iSocket)
{
    QByteArray      lPDU                    = iReceivedDatagram.data();
    QStringList     lRequestedServices;
    QStringList     lRepliedServices;

    if( JCSLP::PDU::V1::getStringList(lPDU,lRequestedServices) ) {
        for( QString lRS : lRequestedServices )
        {
            QString     lRequestedService;
            QString     lRequestedLocation;
            QString     lRequestedProtocol;
            QString     lRequestedAddress;
            quint16     lRequestedPort;

            if( JCSLP::SERVICE::V1::processServiceString(lRS,lRequestedService,lRequestedLocation,lRequestedProtocol,lRequestedAddress,lRequestedPort) ) {
                for( QString lOS : mServices )
                {
                    QString     lOurService;
                    QString     lOurLocation;
                    QString     lOurProtocol;
                    QString     lOurAddress;
                    quint16     lOurPort;

                    if( JCSLP::SERVICE::V1::processServiceString(lOS,lOurService,lOurLocation,lOurProtocol,lOurAddress,lOurPort) ) {
                        // Now lets do our matching
                        if(
                            __filterMatchString(lRequestedService,lOurService)      &&
                            __filterMatchString(lRequestedLocation,lOurLocation)    &&
                            __filterMatchString(lRequestedProtocol,lOurProtocol)    &&
                            __filterMatchString(lRequestedAddress,lOurAddress)      &&
                            __filterMatchPort(lRequestedPort,lOurPort)
                        ) {
                            lRepliedServices << lOS;
                        }
                    }
                }
            }
        }

        if( ! lRepliedServices.isEmpty() ) {
            lPDU = JCSLP::PDU::V1::createUAReply(lRepliedServices);
            if( iSocket != nullptr )
                iSocket->writeDatagram(iReceivedDatagram.makeReply(lPDU));
            else
                QUdpSocket().writeDatagram(iReceivedDatagram.makeReply(lPDU));
        }
    }
}

void JCSLP::SA::_processDAAnnounce(QNetworkDatagram &iReceivedDatagram, QUdpSocket *iSocket)
{
    QByteArray      lPDU                    = iReceivedDatagram.data();
    QStringList     lDAList;

    if( JCSLP::PDU::V1::getStringList(lPDU,lDAList) ) {
        for( QString lDA : lDAList ) {
#error LEFT OFF HERE
        }
    }
}

void JCSLP::SA::_receiveJCSLPPDU()
{
    for( QUdpSocket * lSocket : mSockets )
    {
        QNetworkDatagram            lDatagram   = lSocket->receiveDatagram();
        QByteArray                  lPDU        = lDatagram.data();
        JCSLP::PDU::V1::PDUType     lType;

        if( JCSLP::PDU::V1::getType(lPDU,lType) ) {
            switch( lType ) {
            case JCSLP::PDU::V1::PDUType::UAQuery:
                _processUARequest(lDatagram, lSocket);
                break;

            case JCSLP::PDU::V1::PDUType::DAAnnounce:
                _processDAAnnounce(lDatagram, lSocket);
                break;

            case JCSLP::PDU::V1::PDUType::SAAnnounce:
                if( mDAProxyMode && _inDAMode() ) {
                    // Need to tell the source about our DA list
                    _sendDAListPDU(lDatagram, lSocket);
                }
                break;

            default:
                // We don't care about this type of message
                break;
            }
        }
    }
}

void JCSLP::SA::_heartBeat()
{ _sendSAPDU(); }

JCSLP::SA::SA(const QStringList iServices, const QList<QHostAddress> iListeningInterfaces, QObject *iParent)
    : QObject(iParent)
    , mServices(iServices)
{
    mTimer = new QTimer{this};
    connect( mTimer, &QTimer::timeout, this, &SA::_heartBeat );
    mTimer->setInterval(mHBInterval * 1000);
    mTimer->setSingleShot(false);

    for( QHostAddress lHA : iListeningInterfaces )
    {
        QUdpSocket * lSocket = new QUdpSocket{this};
        lSocket->bind(lHA, kJCSLPPort);
        connect( lSocket, &QUdpSocket::readyRead, this, &SA::_receiveJCSLPPDU );
        mSockets << lSocket;
    }
}

JCSLP::SA::SA(const QString iService, const QList<QHostAddress> iListeningInterfaces, QObject *iParent)
    : SA(QStringList() << iService, iListeningInterfaces, iParent)
{ }

JCSLP::SA::~SA()
{

}

void JCSLP::SA::setHeartBeatInterval(int iInterval)
{
    mHBInterval = iInterval;
    if( mTimer ) {
        mTimer->setInterval(mHBInterval * 1000);
    }
}

int JCSLP::SA::heartBeatInterval() const
{ return mHBInterval; }

void JCSLP::SA::setDAProxyMode(bool iOn)
{ mDAProxyMode = iOn; }

bool JCSLP::SA::daProxyMode() const
{ return mDAProxyMode; }

void JCSLP::SA::start()
{ mTimer->start(); }

void JCSLP::SA::stop()
{ mTimer->stop(); }

void JCSLP::SA::addDA(const QString iDA)
{
    QHostInfo::lookupHost( iDA, [this](QHostInfo &iInfo){
        for( QHostAddress lHA : iInfo.addresses() )
        { addDA(lHA); }
    });
}

void JCSLP::SA::addDA(const QHostAddress iDA)
{
    QWriteLocker l{&mDAListLock};
    mDAStringCacheList = QStringList();
    mDAList.removeAll(iDA);
    mDAList << iDA;
}

void JCSLP::SA::removeDA(const QString iDA)
{
    QHostInfo::lookupHost( iDA, [this](QHostInfo &iInfo){
        for( QHostAddress lHA : iInfo.addresses() )
        { removeDA(lHA); }
    });
}

void JCSLP::SA::removeDA(const QHostAddress iDA)
{
    QWriteLocker l{&mDAListLock};
    mDAStringCacheList = QStringList();
    mDAList.removeAll(iDA);
}
