#include "wcpconnection.h"

WCPConnection::WCPConnection(QWebSocket *iSocket, QObject *iParent)
    : QObject(iParent)
    , mSocket(iSocket)
{
    if( mSocket ) {
        connect( mSocket, &QWebSocket::binaryMessageReceived, this, &WCPConnection::_binaryMessageReceived );
        connect( mSocket, &QWebSocket::textMessageReceived, this, &WCPConnection::_textMessageReceived );
        connect( mSocket, &QWebSocket::disconnected, this, &WCPConnection::disconnected );
    }
}

WCPConnection::WCPConnection(QObject *iParent)
    : QObject(iParent)
{
    mSocket = new QWebSocket;
    mSocket->setParent(this);

    connect( mSocket, &QWebSocket::binaryMessageReceived, this, &WCPConnection::_binaryMessageReceived );
    connect( mSocket, &QWebSocket::textMessageReceived, this, &WCPConnection::_textMessageReceived );
    connect( mSocket, &QWebSocket::disconnected, this, &WCPConnection::disconnected );
    connect( mSocket, &QWebSocket::connected, this, &WCPConnection::connected );
    connect( mSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), this, &WCPConnection::socketError );
    connect( mSocket, &QWebSocket::sslErrors, this, &WCPConnection::sslErrors );
}

QWebSocket *WCPConnection::socket() const
{ return mSocket; }

bool WCPConnection::haveBinaryMessages() const
{
    QMutexLocker l{&mBinaryMessagesLock};
    return (mBinaryMessages.isEmpty() ? false : true );
}

bool WCPConnection::haveTextMessages() const
{
    QMutexLocker l{&mTextMessagesLock};
    return (mTextMessages.isEmpty() ? false : true );
}

int WCPConnection::binaryMessageCount() const
{
    QMutexLocker l{&mBinaryMessagesLock};
    return mBinaryMessages.count();
}

int WCPConnection::textMessageCount() const
{
    QMutexLocker l{&mTextMessagesLock};
    return mTextMessages.count();
}

QByteArray WCPConnection::nextBinaryMessage()
{
    QMutexLocker l{&mBinaryMessagesLock};
    if( ! mBinaryMessages.isEmpty() )
        return mBinaryMessages.takeFirst();
    return QByteArray();
}

QString WCPConnection::nextTextMessage()
{
    QMutexLocker l{&mTextMessagesLock};
    if( ! mTextMessages.isEmpty() )
        return mTextMessages.takeFirst();
    return QString();
}

bool WCPConnection::isConnected() const
{
    if( mSocket && QAbstractSocket::ConnectedState == mSocket->state() ) return true;
    return false;
}

void WCPConnection::open(const QUrl &iUrl)
{
    if(mSocket) {
        if( mSocket->thread() == thread() )
            mSocket->open(iUrl);
        else
            QMetaObject::invokeMethod(mSocket,"open",Qt::QueuedConnection);
    }
}

void WCPConnection::close()
{
    if(mSocket) {
        if( mSocket->thread() == thread() )
            mSocket->close();
        else
            QMetaObject::invokeMethod(mSocket,"close",Qt::QueuedConnection);
    }
}

bool WCPConnection::sendBinaryMessage(const QByteArray &iMessage)
{
    bool lSuccess = false;

    mSendLock.lock();
    if( mSocket )
        lSuccess = (mSocket->sendBinaryMessage(iMessage) == iMessage.size());
    mSendLock.unlock();

    if( lSuccess )
        emit sentBinaryMessage();
    else
        emit failedToSendBinaryMessage();

    return lSuccess;
}

bool WCPConnection::sendTextMessage(const QString &iMessage)
{
    bool lSuccess = false;

    mSendLock.lock();
    if( mSocket )
        lSuccess = (mSocket->sendTextMessage(iMessage) == iMessage.size());
    mSendLock.unlock();

    if( lSuccess )
        emit sentTextMessage();
    else
        emit failedToSendTextMessage();

    return lSuccess;
}

void WCPConnection::setSslConfiguration(const QSslConfiguration iConfiguration)
{
    mSocket->setSslConfiguration(iConfiguration);
}

void WCPConnection::_binaryMessageReceived(const QByteArray &iMessage)
{
    mBinaryMessagesLock.lock();
    mBinaryMessages << iMessage;
    mBinaryMessagesLock.unlock();

    emit binaryMessageReceived();
}

void WCPConnection::_textMessageReceived(const QString &iMessage)
{
    mTextMessagesLock.lock();
    mTextMessages << iMessage;
    mTextMessagesLock.unlock();

    emit textMessageReceived();
}
