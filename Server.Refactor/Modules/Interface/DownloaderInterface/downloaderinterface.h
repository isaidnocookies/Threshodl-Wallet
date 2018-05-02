#ifndef DOWNLOADERINTERFACE_H
#define DOWNLOADERINTERFACE_H

#include <QReadWriteLock>
#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkRequest>

class DownloaderInterface : public QObject
{
protected:
    mutable QReadWriteLock      mUrlsLock;
    QList<QUrl>                 mUrls;

public:
    typedef QNetworkRequest (*UrlCallback_t)(const QUrl iUrl, void * iUserData);

    DownloaderInterface(QObject * iParent = nullptr) : QObject(iParent)
    { }

    virtual ~DownloaderInterface()
    { }

    QList<QUrl> urls() const                            { QReadLocker l{&mUrlsLock}; return mUrls; }

public slots:
    // Url callbacks are placed on allready added Urls or if the Url is not already added then it adds it as it is placed
    virtual void    addUrlCallback(const QUrl iUrl, UrlCallback_t iCallback, void * iUserData = nullptr)    = 0;
    virtual void    removeUrlCallback(const QUrl iUrl)                                                      = 0;

    virtual void    removeUrl(const QUrl iUrl)          { removeUrlCallback(iUrl); QWriteLocker l{&mUrlsLock}; mUrls.removeAll(iUrl); }
    virtual void    addUrl(const QUrl iUrl)             { removeUrl(iUrl); QWriteLocker l{&mUrlsLock}; mUrls << iUrl; }
    virtual void    setUrls(const QList<QUrl> iUrls)    { QWriteLocker l{&mUrlsLock}; mUrls = iUrls; }

signals:
    virtual void    downloaded( const QUrl iUrl, const QByteArray iData )   = 0;
    virtual void    timedOut( const QUrl iUrl )                             = 0;
    virtual void    failed( const QUrl iUrl )                               = 0;
};

#endif // DOWNLOADERINTERFACE_H
