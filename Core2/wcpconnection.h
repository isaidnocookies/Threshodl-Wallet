#ifndef WCPCONNECTION_H
#define WCPCONNECTION_H

#include <QByteArray>
#include <QMutex>
#include <QList>
#include <QWebSocket>

class WCPConnection : public QObject
{
    Q_OBJECT
public:
    explicit WCPConnection(QWebSocket * iSocket, QObject * iParent = nullptr);
    explicit WCPConnection(QObject * iParent = nullptr); // only used from a client connection

    QWebSocket * socket() const;
    bool haveBinaryMessages() const;
    bool haveTextMessages() const;
    int binaryMessageCount() const;
    int textMessageCount() const;
    QByteArray nextBinaryMessage();
    QString nextTextMessage();
    bool isConnected() const;

signals:
    void connected(); // Only emitted from a client connection
    void disconnected();
    void binaryMessageReceived();
    void textMessageReceived();
    void failedToSendBinaryMessage();
    void sentBinaryMessage();
    void failedToSendTextMessage();
    void sentTextMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

public slots:
    void open(const QUrl &iUrl); // only used from a client connection
    void close();
    bool sendBinaryMessage(const QByteArray &iMessage);
    bool sendTextMessage(const QString &iMessage);
    void setSslConfiguration(const QSslConfiguration iConfiguration);

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

#endif // WCPCONNECTION_H
