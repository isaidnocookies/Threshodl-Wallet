#ifndef WCPCLIENT_H
#define WCPCLIENT_H

#include "../../Interface/WCPInterface/wcpclientinterface.h"
#include "wcpmessages.h"
#include "qstringmath.h"

#include <QSharedPointer>
#include <QElapsedTimer>

class WCPServerHandler;
class WCPClient : public WCPClientInterface
{
    Q_OBJECT
protected:
    typedef struct {
        bool                DelayedProcessing;
        QElapsedTimer       Timer;
        QString             TransactionId;
    } PendingNodeRelayOp;

    WCPServerHandler *                                          mServerV1;
    QMap< QNetworkReply *, QSharedPointer<PendingNodeRelayOp> > mPendingNodeRelayOperations;
    int                                                         mPendingNodeRelayOperationsTimeOutInMS          = 30000;
    QTimer *                                                    mPendingNodeRelayOperationsCleanUpTimer         = nullptr;
    int                                                         mPendingNodeRelayOperationsCleanUpTimerInterval = 5000;

    void _startPendingNodeRelayOperationsCleanUpTimer();
    void _stopPendingNodeRelayOperationsCleanUpTimer();

    void _getBTCFees(BitcoinWallet::ChainType iChain, QStringMath &oBaseFee, QStringMath &oInputFee, QStringMath &oOutputFee);

    bool _authenticateMessage(const WCPMessage &iMessage);
    void _createMicroWallets(const WCPMessage &iMessage);
    void _createMicroWalletsBTC(const WCPMessage &iMessage);
    void _claimNewMicroWallets(const WCPMessage &iMessage);
    void _checkOwnershipOfMicroWallets(const WCPMessage &iMessage);
    void _completeMicroWallets(const WCPMessage &iMessage);
    void _createAccount(const WCPMessage &iMessage);
    void _reassignMicroWallets(const WCPMessage &iMessage);
    void _ping(const WCPMessage &iMessage);
    void _sendNodeRestMessage(const WCPMessage &iMessage);

protected slots:
    void _nodeRelayCleanUpTimer();
    void _nodeRelayFinished();

public:
    explicit WCPClient(WCPConnection * iConnection, WCPServerHandler * iServer, QObject *iParent = nullptr);

    virtual ~WCPClient()
    { }

    virtual void processMessage(const QString iWCPVersion, const QString iCommand, const WCPMessage &iMessage);
};


#endif // WCPCLIENT_H
