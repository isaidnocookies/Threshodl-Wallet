#ifndef RESTNODERELAY_H
#define RESTNODERELAY_H

#include "../../Interface/RESTNodeRelayInterface/restnoderelayinterface.h"

#include <QMap>
#include <QNetworkAccessManager>

class App;
class RESTNodeRelayML;
class RESTNodeRelay : public RESTNodeRelayInterface
{
    friend class RESTNodeRelayML;
protected:
    App *                               mApp                = nullptr;
    QNetworkAccessManager *             mNAM                = nullptr;
    QMap< QString, QString >            mCryptoToUrlBase;

public:
    RESTNodeRelay();
    virtual ~RESTNodeRelay();

    QNetworkReply * get(const QString iCryptoTypeShortName, const QString iUrlPath) override;
    QNetworkReply * post(const QString iCryptoTypeShortName, const QString iUrlPath, const QString iPayloadType, const QByteArray iPayload) override;
};

class RESTNodeRelayML
{
public:
    RESTNodeRelayML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // RESTNODERELAY_H
