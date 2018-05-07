#ifndef JCSLPSA_H
#define JCSLPSA_H

#include <QString>
#include <QList>
#include <QHostAddress>
#include <QUdpSocket>
#include <QTimer>
#include <QReadWriteLock>
#include <QElapsedTimer>

namespace JCSLP
{
class SA : public QObject
{
    Q_OBJECT
protected:
    int                     mHBInterval         = 5;
    QStringList             mServices;
    QList< QUdpSocket * >   mSockets;
    QTimer *                mTimer              = nullptr;
    QByteArray              mSAPDU;

    bool                    mDAProxyMode        = true;

    mutable QReadWriteLock  mDAListLock;
    bool                    mInDAMode           = false;
    QStringList             mDAStringCacheList;
    QList< QHostAddress >   mDAList;

    bool            _inDAMode() const;

protected slots:
    void            _sendSAPDU(QUdpSocket *iSocket = nullptr);
    void            _sendDAListPDU(QNetworkDatagram &iReceivedDatagram, QUdpSocket *iSocket = nullptr);
    void            _processUARequest(QNetworkDatagram &iReceivedDatagram, QUdpSocket *iSocket = nullptr);
    void            _processDAAnnounce(QNetworkDatagram &iReceivedDatagram, QUdpSocket *iSocket = nullptr);
    void            _receiveJCSLPPDU();
    void            _heartBeat();

public:
    SA(const QStringList iServices, const QList<QHostAddress> iListeningInterfaces = QList<QHostAddress>() << QHostAddress::Any, QObject * iParent = nullptr);
    SA(const QString iService, const QList<QHostAddress> iListeningInterfaces = QList<QHostAddress>() << QHostAddress::Any, QObject * iParent = nullptr);

    ~SA();

    int heartBeatInterval() const;  // In seconds
    bool daProxyMode() const;

public slots:
    void start();
    void stop();

    void setHeartBeatInterval(int iInterval);
    void setDAProxyMode(bool iOn = true);
    void addDA(const QString iDA);
    void addDA(const QHostAddress iDA);
    void removeDA(const QString iDA);
    void removeDA(const QHostAddress iDA);
};
}

#endif // JCSLPSA_H
