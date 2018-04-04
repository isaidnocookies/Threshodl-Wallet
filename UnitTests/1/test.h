#ifndef TEST_H
#define TEST_H

#include "rpcconnection.h"
#include <QObject>

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(RPCConnection * iConnection, QObject *iParent = nullptr);

signals:

public slots:
    void rpcConnected();
};

#endif // TEST_H
