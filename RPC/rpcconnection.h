#ifndef RPCCONNECTION_H
#define RPCCONNECTION_H

#include <QByteArray>
#include <QMutex>
#include <QList>
#include <QWebSocket>

class RPCConnection : public QObject
{
    Q_OBJECT
public:
    explicit RPCConnection(QWebSocket * iSocket, QObject * iParent = nullptr);
    explicit RPCConnection(QObject * iParent = nullptr); // only used from a client connection

    QWebSocket * socket() const;
    bool haveBinaryMessages() const;
    bool haveTextMessages() const;
    int binaryMessageCount() const;
    int textMessageCount() const;
    QByteArray nextBinaryMessage();
    QString nextTextMessage();

signals:
    void connected(); // Only emitted from a client connection
    void disconnected();
    void binaryMessageReceived();
    void textMessageReceived();
    void failedToSendBinaryMessage();
    void sentBinaryMessage();
    void failedToSendTextMessage();
    void sentTextMessage();

public slots:
    void open(const QUrl &iUrl); // only used from a client connection
    void close();
    void sendBinaryMessage(const QByteArray &iMessage);
    void sendTextMessage(const QString &iMessage);

protected slots:
    void _binaryMessageReceived(const QByteArray &iMessage);
    void _textMessageReceived(const QString &iMessage);

private:
    mutable QMutex          mSendLock;
    QWebSocket *            mSocket = nullptr;
    mutable QMutex          mBinaryMessagesLock;
    QList< QByteArray >     mBinaryMessages;
    mutable QMutex          mTextMessagesLock;
    QList< QString >        mTextMessages;
};

#endif // RPCCONNECTION_H
