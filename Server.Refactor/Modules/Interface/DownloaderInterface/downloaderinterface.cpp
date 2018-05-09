#include "downloaderinterface.h"

DownloaderInterface::DownloaderInterface(QObject *iParent) : QObject(iParent)
{ }

DownloaderInterface::~DownloaderInterface()
{ }

QList<QUrl> DownloaderInterface::urls() const
{
    QReadLocker l{&mUrlsLock};
    return mUrls;
}

void DownloaderInterface::removeUrl(const QUrl iUrl)
{
    removeUrlCallback(iUrl);
    mUrlsLock.lockForWrite();
    mUrls.removeAll(iUrl);
    mUrlsLock.unlock();
    emit urlsChanged();
}

void DownloaderInterface::addUrl(const QUrl iUrl)
{
    removeUrl(iUrl);
    mUrlsLock.lockForWrite();
    mUrls << iUrl;
    mUrlsLock.unlock();
    emit urlsChanged();
}

void DownloaderInterface::setUrls(const QList<QUrl> iUrls)
{
    mUrlsLock.lockForWrite();
    mUrls = iUrls;
    mUrlsLock.unlock();
    emit urlsChanged();
}
