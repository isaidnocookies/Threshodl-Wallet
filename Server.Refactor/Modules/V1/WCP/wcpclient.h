#ifndef WCPCLIENT_H
#define WCPCLIENT_H

#include "../../Interface/WCPInterface/wcpclientinterface.h"
#include "wcpmessages.h"
#include "qstringmath.h"

class WCPServerHandler;
class WCPClient : public WCPClientInterface
{
    Q_OBJECT
protected:
    WCPServerHandler *  mServerV1;

    void _getBTCFees(BitcoinWallet::ChainType iChain, QStringMath &oBaseFee, QStringMath &oInputFee, QStringMath &oOutputFee);

    bool _authenticateMessage(const WCPMessage &iMessage);
    void _createMicroWalletPackage(const WCPMessage &iMessage);
    void _createMicroWalletPackageBTC(const WCPMessage &iMessage);
    void _checkOwnershipOfMicroWallets(const WCPMessage &iMessage);
    void _completeMicroWallets(const WCPMessage &iMessage);
    void _createAccount(const WCPMessage &iMessage);
    void _reassignMicroWallets(const WCPMessage &iMessage);
    void _ping(const WCPMessage &iMessage);

public:
    explicit WCPClient(WCPConnection * iConnection, WCPServerHandler * iServer, QObject *iParent = nullptr);

    virtual ~WCPClient()
    { }

    virtual void processMessage(const QString iWCPVersion, const QString iCommand, const WCPMessage &iMessage);
};


#endif // WCPCLIENT_H
