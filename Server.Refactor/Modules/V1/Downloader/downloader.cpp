#include "downloader.h"
#include "app.h"

#include <QMessageAuthenticationCode>

static DownloaderML _gRegisterModuleLinker;

Downloader::Downloader(QObject *iParent)
    : DownloaderInterface(iParent)
{ }

Downloader::~Downloader()
{
    if( mTimer ) {
        mTimer->deleteLater();
        mTimer = nullptr;
    }

    if( mNAM ) {
        mNAM->deleteLater();
        mNAM = nullptr;
    }
}

void Downloader::addUrlCallback(const QUrl iUrl, DownloaderInterface::UrlCallback_t iCallback, void *iUserData)
{
    PendingUrlCallback  lPending;
    lPending.Callback           = iCallback;
    lPending.CallbackUserData   = iUserData;

    mDownloadEventsLock.lock();
    mNewUrlCallbacks[iUrl] = lPending;
    mDownloadEventsLock.unlock();

    emit urlsChanged();
}

void Downloader::removeUrlCallback(const QUrl iUrl)
{
    QMutexLocker l{&mDownloadEventsLock};
    mNewUrlCallbacks.remove(iUrl);
    if( mDownloadEvents.contains(iUrl) ) {
        auto lDE = mDownloadEvents[iUrl];
        lDE->Callback           = nullptr;
        lDE->CallbackUserData   = nullptr;
        mDownloadEvents[iUrl] = lDE;
    }
}

void Downloader::_urlsChanged()
{
    // What we need to do here is iterate through our list of Urls and create new "DownloadEvent" objects and also
    // remove "DownloadEvent" objects of deleted objects. Finally we must also glue any pending callbacks into
    // these new "DownloadEvent" objects

    QMutexLocker    lL{&mDownloadEventsLock};
    QWriteLocker    lP{&mUrlsLock};

    // First add any Urls found in our mNewUrlCallbacks in case they are not already there
    for( auto lUrl : mNewUrlCallbacks.keys() ) {
        if( ! mUrls.contains(lUrl) )
            mUrls << lUrl;
    }

    // Add any missing mDownloadEvents
    for( auto lUrl : mUrls )
    {
        if( mDownloadEvents.keys().contains(lUrl) ) continue;

        QSharedPointer<DownloadEvent> lE = QSharedPointer<DownloadEvent>(new DownloadEvent);
        lE->Url                 = lUrl;
        lE->Busy                = false;
        lE->Reply               = nullptr;
        lE->Callback            = nullptr;
        lE->CallbackUserData    = nullptr;

        if( mNewUrlCallbacks.contains(lUrl) ) {
            auto lPC = mNewUrlCallbacks.take(lUrl);
            lE->Callback = lPC.Callback;
            lE->CallbackUserData = lPC.CallbackUserData;
        }

        lE->Timer.start();
        mDownloadEvents[lUrl] = lE;
    }

    // Now delete
    for( auto lDEK : mDownloadEvents.keys() )
    {
        if( mUrls.contains(lDEK) ) continue;

        auto lDE = mDownloadEvents.take(lDEK);

        if( lDE->Reply ) {
            lDE->Reply->deleteLater();
            lDE->Reply = nullptr;
        }
    }
}

void Downloader::_timerFired()
{
    QList<QUrl> lTimedOutUrls;

    mDownloadEventsLock.lock();
    for( auto lDE : mDownloadEvents.values() ) {
        if( lDE->Busy ) {
            // Check for timeout
            if( lDE->Timer.hasExpired(mDownloadTimeOutInMS) || lDE->Reply == nullptr || ! lDE->Reply->isRunning() ) {
                // Timed out
                if( lDE->Reply != nullptr ) {
                    disconnect( lDE->Reply, NULL, this, NULL );
                    lDE->Reply->deleteLater();
                    lDE->Reply = nullptr;
                }
                lDE->Busy = false;

                lTimedOutUrls << lDE->Url;
            }
        }

        // If not busy after timeout check and possible reset from timeout
        if( ! lDE->Busy) {
            // Start download
            if( lDE->Reply ) {
                // Clean up old object
                disconnect( lDE->Reply, NULL, this, NULL );
                lDE->Reply->deleteLater();
                lDE->Reply = nullptr;
            }

//            QNetworkRequest lRequest = QNetworkRequest(lDE->Url);
//            QString         lDigest = QStringLiteral("%1.%2").arg(QDateTime::currentSecsSinceEpoch()).arg(QStringLiteral("YTE2NzlkODZiZjhkNDMyY2FjNzRlYTMwNGUxZWFhMDU"));
//            QByteArray      lKey = "M2VjN2JmZDBiYTU1NDUwNzk4MzZiNWU2Y2NmYzNkNjRhMjAzMWM1NmNkODU0MTc0YWI5MGQ5OWUwNTY5NDg5MQ";
//            QByteArray      lDigestHMAC = QMessageAuthenticationCode::hash(lDigest.toUtf8(), lKey, QCryptographicHash::Sha256).toHex();
//            QByteArray      lHeaderValue = (QStringLiteral("%1.%2").arg(lDigest).arg(lDigestHMAC.constData())).toUtf8();
//            lRequest.setRawHeader("X-signature", lHeaderValue);

//            lDE->Reply = mNAM->get(QNetworkRequest(lRequest));

            lDE->Reply = mNAM->get(QNetworkRequest{lDE->Url});

            lDE->Timer.restart();
            lDE->Busy = true;

            connect( lDE->Reply, QOverload<>::of(&QNetworkReply::finished),
                     [this,lDE](){
                this->_networkReplyFinished(lDE->Url,lDE->Reply);
            });

            connect( lDE->Reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                     [this,lDE](QNetworkReply::NetworkError iErrorCode){
                this->_networkReplyError(lDE->Url,lDE->Reply,iErrorCode);
            });
        }
    }
    mDownloadEventsLock.unlock();

    for( QUrl lTimedOutUrl : lTimedOutUrls ) {
        emit timedOut(lTimedOutUrl);
    }
}

void Downloader::_restartTimer()
{
    if( mTimer ) {
        if( mTimer->thread() != QThread::currentThread() ) {
            QMetaObject::invokeMethod( mTimer, "stop", Qt::BlockingQueuedConnection );
            QMetaObject::invokeMethod( mTimer, "start", Qt::BlockingQueuedConnection );
        }else{
            mTimer->stop();
            mTimer->start();
        }
    }
}

void Downloader::_networkReplyFinished(const QUrl iUrl, QNetworkReply *iSource)
{
    if( iSource == nullptr ) return;

    QByteArray  lData = iSource->readAll();

    mDownloadEventsLock.lock();
    auto lDE = mDownloadEvents[iUrl];
    lDE->Busy = false;
    lDE->Reply->deleteLater();
    lDE->Reply = nullptr;
    mDownloadEventsLock.unlock();

    if( ! lData.isEmpty() ) {
        qDebug() << "Downloaded" << lData.size() << "bytes from" << iUrl.toString();
        emit downloaded(iUrl, lData);
    }else{
        qDebug() << "Failed to download from" << iUrl.toString();
        emit failed(iUrl);
    }
}

void Downloader::_networkReplyError(const QUrl iUrl, QNetworkReply *iSource, QNetworkReply::NetworkError iErrorCode)
{
    Q_UNUSED(iSource) Q_UNUSED(iErrorCode)

    mDownloadEventsLock.lock();
    auto lDE = mDownloadEvents[iUrl];
    lDE->Busy = false;
    lDE->Reply->deleteLater();
    lDE->Reply = nullptr;
    mDownloadEventsLock.unlock();

    emit failed(iUrl);
}

bool Downloader::loadConfiguration()
{
    if( mApp ) {
        auto lConfig = mApp->configuration();

        if( lConfig->contains(QStringLiteral("DownloaderInterval")) )
            mTimerIntervalInMS = lConfig->toInt32(QStringLiteral("DownloaderInterval"));

        if( lConfig->contains(QStringLiteral("DownloaderTimeOut")) )
            mDownloadTimeOutInMS = lConfig->toUInt32(QStringLiteral("DownloaderTimeOut"));

        setUrls(QUrl::fromStringList(lConfig->toStringList(QStringLiteral("DownloadUrls"))));

        for( QUrl lUrl : urls() ) {
            QSharedPointer<DownloadEvent> lE = QSharedPointer<DownloadEvent>(new DownloadEvent);
            lE->Url                 = lUrl;
            lE->Busy                = false;
            lE->Reply               = nullptr;
            lE->Callback            = nullptr;
            lE->CallbackUserData    = nullptr;
            lE->Timer.start();
            mDownloadEvents[lUrl] = lE;
        }

        if( mTimerIntervalInMS < 1 || mDownloadTimeOutInMS < 1 )
            return false;

        return true;
    }

    return false;
}

void Downloader::connectInternalSignalsAndSlots()
{
    connect( mTimer, &QTimer::timeout, this, &Downloader::_timerFired );
    connect( this, &Downloader::urlsChanged, this, &Downloader::_urlsChanged );
}

DownloaderML::DownloaderML()
{
    ModuleLinker::registerModule(QStringLiteral("Downloader-1"),DownloaderML::creator,DownloaderML::doInit,DownloaderML::start,DownloaderML::startInOwnThread);
}

void *DownloaderML::creator(void *pointerToAppObject)
{
    Downloader * lDM = new Downloader;
    lDM->mApp = reinterpret_cast<App *>(pointerToAppObject);
    return lDM;
}

bool DownloaderML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToThis) Q_UNUSED(pointerToAppObject)
    return true;
}

bool DownloaderML::startInOwnThread()
{ return true; }

bool DownloaderML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)

    Downloader * lDM = reinterpret_cast<Downloader *>(pointerToThis);

    lDM->mTimer = new QTimer{lDM};
    lDM->mTimer->setInterval(lDM->mTimerIntervalInMS);
    lDM->mTimer->setSingleShot(false);

    lDM->mNAM   = new QNetworkAccessManager{lDM};

    if( ! lDM->loadConfiguration() ) return false;
    lDM->connectInternalSignalsAndSlots();
    lDM->_restartTimer();
    return true;
}
