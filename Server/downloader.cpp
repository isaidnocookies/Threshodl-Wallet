#include "downloader.h"

Downloader::Downloader(QObject * iParent)
    : QObject(iParent)
{

}

QStringList Downloader::urls() const
{
    return mURLs;
}

bool Downloader::isCurrentlyDownloading(const QString &iUrl) const
{
    QMutexLocker l{&mDownloadEventsLock};
    if( mDownloadEvents.contains(iUrl) ) {
        return mDownloadEvents[iUrl]->Busy;
    }
    return false;
}

void Downloader::threadStarted()
{
    mNAM    = new QNetworkAccessManager(this);
    mURLs   << QStringLiteral("https://apiv2.bitcoinaverage.com/indices/global/ticker/BTCUSD")
            << QStringLiteral("https://apiv2.bitcoinaverage.com/indices/global/ticker/ETHUSD")
            << QStringLiteral("https://apiv2.bitcoinaverage.com/indices/crypto/ticker/ETHBTC");

    for( QString lUrl : mURLs ) {
        QSharedPointer<DownloadEvent> lDE = QSharedPointer<DownloadEvent>(new DownloadEvent);
        lDE->Timer.start();
        lDE->UrlString          = lUrl;
        lDE->Url                = QUrl::fromUserInput(lUrl);
        lDE->Busy               = false;
        lDE->Reply              = nullptr;
        mDownloadEvents[lUrl]   = lDE;
    }

    mTimer  = new QTimer(this);
    connect( mTimer, &QTimer::timeout, this, &Downloader::timerEvent );
    mTimer->setSingleShot(false);
    mTimer->setInterval(15000);
    mTimer->start();
}

void Downloader::timerEvent()
{
    QStringList     lTimedOutUrls;

    mDownloadEventsLock.lock();
    for( auto lDE : mDownloadEvents.values() ) {
        if( lDE->Busy ) {
            // Check for timeout
            if( lDE->Timer.hasExpired(mTimeOut) || lDE->Reply == nullptr || ! lDE->Reply->isRunning() ) {
                // Timed out
                if( lDE->Reply != nullptr ) {
                    disconnect( lDE->Reply, NULL, this, NULL );
                    lDE->Reply->deleteLater();
                    lDE->Reply = nullptr;
                }
                lDE->Busy = false;

                lTimedOutUrls << lDE->UrlString;
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

            lDE->Reply = mNAM->get(QNetworkRequest(lDE->Url));
            lDE->Timer.restart();
            lDE->Busy = true;

            connect( lDE->Reply, QOverload<>::of(&QNetworkReply::finished),
                     [this,lDE](){
                this->networkReplyFinished(lDE->UrlString,lDE->Reply);
            });

            connect( lDE->Reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
                     [this,lDE](QNetworkReply::NetworkError iErroCode){
                this->networkReplyError(lDE->UrlString,lDE->Reply,iErroCode);
            });
        }
    }
    mDownloadEventsLock.unlock();

    for( QString lTimedOutUrl : lTimedOutUrls ) {
        emit timedOut(lTimedOutUrl);
    }
}

void Downloader::networkReplyFinished(const QString iUrl, QNetworkReply *iSource)
{
    QByteArray  lData = iSource->readAll();

    mDownloadEventsLock.lock();
    auto lDE = mDownloadEvents[iUrl];
    lDE->Busy = false;
    lDE->Reply->deleteLater();
    lDE->Reply = nullptr;
    mDownloadEventsLock.unlock();

    if( lData.isEmpty() ) {
        emit downloaded(iUrl, lData);
    }else{
        emit failed(iUrl);
    }
}

void Downloader::networkReplyError(const QString iUrl, QNetworkReply *iSource, QNetworkReply::NetworkError iErrorCode)
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
