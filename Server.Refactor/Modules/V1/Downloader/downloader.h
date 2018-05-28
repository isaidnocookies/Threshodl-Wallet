#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "../../Interface/DownloaderInterface/downloaderinterface.h"

#include <QElapsedTimer>
#include <QMutex>
#include <QSharedPointer>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkReply>

class DownloaderML
{
public:
    DownloaderML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

class App;
class Downloader : public DownloaderInterface
{
    Q_OBJECT
    friend class DownloaderML;
public:
    Downloader(QObject * iParent = nullptr);
    virtual ~Downloader();

public slots:
    void addUrlCallback(const QUrl iUrl, UrlCallback_t iCallback, void * iUserData = nullptr) override;
    void removeUrlCallback(const QUrl iUrl) override;

protected slots:
    void    _urlsChanged();
    void    _timerFired();
    void    _restartTimer();
    void    _networkReplyFinished(const QUrl iUrl, QNetworkReply * iSource );
    void    _networkReplyError(const QUrl iUrl, QNetworkReply * iSource, QNetworkReply::NetworkError iErrorCode );

protected:
    bool loadConfiguration();
    void connectInternalSignalsAndSlots();

    typedef struct
    {
        QElapsedTimer       Timer;
        QUrl                Url;
        bool                Busy;
        QNetworkReply *     Reply;
        UrlCallback_t       Callback;
        void *              CallbackUserData;
    } DownloadEvent;

    typedef struct
    {
        UrlCallback_t       Callback;
        void *              CallbackUserData;
    } PendingUrlCallback;

    App *                                           mApp                    = nullptr;
    QTimer *                                        mTimer                  = nullptr;
    QNetworkAccessManager *                         mNAM                    = nullptr;
    int                                             mTimerIntervalInMS      = 5000;
    quint64                                         mDownloadTimeOutInMS    = 120000;

    mutable QMutex                                  mDownloadEventsLock;
    QMap< QUrl, QSharedPointer<DownloadEvent> >     mDownloadEvents;
    QMap< QUrl, PendingUrlCallback >                mNewUrlCallbacks;       // These are new Urls waiting for callbacks to be attached
};

#endif // DOWNLOADER_H
