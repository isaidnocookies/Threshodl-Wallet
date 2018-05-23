#ifndef WCPSERVERHANDLER_H
#define WCPSERVERHANDLER_H

#include "../../Interface/WCPInterface/wcpserverhandlerinterface.h"
#include "../../Interface/DatabaseInterface/databaseinterface.h"
#include "../../Interface/GrinderInterface/grinderinterface.h"
#include "../../Interface/FeeEstimatorInterface/feeestimatorinterface.h"
#include "../../Interface/RESTNodeRelayInterface/restnoderelayinterface.h"

#include <QMutex>
#include <QList>
#include <QObject>

class App;
class WCPClientInterface;
class WCPServerHandlerML;
class WCPServerHandler : public WCPServerHandlerInterface
{
    Q_OBJECT
    friend class WCPServerHandlerML;
protected:
    App *                       mApp;
    DatabaseInterface *         mDB;
    GrinderInterface *          mGrinder;
    FeeEstimatorInterface *     mFeeEstimator;
    RESTNodeRelayInterface *    mRESTNodeRelay;

    quint16                     mPort;
    QStringList                 mServerNames;
    QHostAddress                mListenAddress          = QHostAddress::Any;
    QSslConfiguration           mSslConfiguration;

public:
    explicit WCPServerHandler(QObject * iParent = nullptr);
    virtual ~WCPServerHandler();

    App *                       app() const                 { return mApp; }
    DatabaseInterface *         database() const            { return mDB; }
    GrinderInterface *          grinder() const             { return mGrinder; }
    FeeEstimatorInterface *     feeEstimator() const        { return mFeeEstimator; }
    RESTNodeRelayInterface *    restNodeRelay() const       { return mRESTNodeRelay; }

public slots:
    virtual void serverStarted();
    virtual void serverStopped();
    virtual void serverFailedToStart();
    virtual void serverFailedToStop();
    virtual void newConnectionArrived();
};

class WCPServerHandlerML
{
public:
    WCPServerHandlerML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // WCPSERVERHANDLER_H
