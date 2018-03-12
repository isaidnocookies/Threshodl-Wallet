#include "rpcconnection.h"

RPCConnection::RPCConnection(QWebSocket *iSocket, QObject *iParent)
    : QObject(iParent)
    , mSocket(iSocket)
{
    if( mSocket ) {
        connect( mSocket, &QWebSocket::binaryMessageReceived, this, &RPCConnection::_binaryMessageReceived );
        connect( mSocket, &QWebSocket::textMessageReceived, this, &RPCConnection::_textMessageReceived );
        connect( mSocket, &QWebSocket::disconnected, this, &RPCConnection::disconnected );
    }
}

QWebSocket *RPCConnection::socket() const
{ return mSocket; }

bool RPCConnection::haveBinaryMessages() const
{
    QMutexLocker l{&mBinaryMessagesLock};
    return (mBinaryMessages.isEmpty() ? false : true );
}

bool RPCConnection::haveTextMessages() const
{
    QMutexLocker l{&mTextMessagesLock};
    return (mTextMessages.isEmpty() ? false : true );
}

int RPCConnection::binaryMessageCount() const
{
    QMutexLocker l{&mBinaryMessagesLock};
    return mBinaryMessages.count();
}

int RPCConnection::textMessageCount() const
{
    QMutexLocker l{&mTextMessagesLock};
    return mTextMessages.count();
}

QByteArray RPCConnection::nextBinaryMessage()
{
    QMutexLocker l{&mBinaryMessagesLock};
    if( ! mBinaryMessages.isEmpty() )
        return mBinaryMessages.takeFirst();
    return QByteArray();
}

QString RPCConnection::nextTextMessage()
{
    QMutexLocker l{&mTextMessagesLock};
    if( ! mTextMessages.isEmpty() )
        return mTextMessages.takeFirst();
    return QString();
}

void RPCConnection::sendBinaryMessage(const QByteArray &iMessage)
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
}

void RPCConnection::sendTextMessage(const QString &iMessage)
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
}

void RPCConnection::_binaryMessageReceived(const QByteArray &iMessage)
{
    mBinaryMessagesLock.lock();
    mBinaryMessages << iMessage;
    mBinaryMessagesLock.unlock();

    emit binaryMessageReceived();
}

void RPCConnection::_textMessageReceived(const QString &iMessage)
{
    mTextMessagesLock.lock();
    mTextMessages << iMessage;
    mTextMessagesLock.unlock();

    emit textMessageReceived();
}
