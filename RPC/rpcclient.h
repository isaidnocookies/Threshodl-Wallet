#ifndef RPCCLIENT_H
#define RPCCLIENT_H

#ifdef ENABLE_RPCCLIENT

#include <QObject>

class RPCClient : public QObject
{
    Q_OBJECT
public:
    explicit RPCClient(QObject * iParent = nullptr);

signals:

public slots:
};

#endif // ENABLE_RPCCLIENT

#endif // RPCCLIENT_H
