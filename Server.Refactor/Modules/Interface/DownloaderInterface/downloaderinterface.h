#ifndef DOWNLOADERINTERFACE_H
#define DOWNLOADERINTERFACE_H

#include <QReadWriteLock>
#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QObject>

class DownloaderInterface : public QObject
{
protected:
    mutable QReadWriteLock      mUrlsLock;
    QList<QUrl>                 mUrls;

public:
    DownloaderInterface(QObject * iParent = nullptr) : QObject(iParent)
    { }

    virtual ~DownloaderInterface()
    { }

    QList<QUrl> urls() const                            { QReadLocker l{&mUrlsLock}; return mUrlsLock; }

public slots:
    virtual void    addUrl(const QUrl iUrl)             { QWriteLocker l{&mUrlsLock}; mUrls << iUrl; }
    virtual void    setUrls(const QList<QUrl> iUrls)    { QWriteLocker l{&mUrlsLock}; mUrls = iUrls; }

signals:
    virtual void    downloaded( const QUrl iUrl, const QByteArray iData )   = 0;
    virtual void    timedOut( const QUrl iUrl )                             = 0;
    virtual void    failed( const QUrl iUrl )                               = 0;
}

#endif // DOWNLOADERINTERFACE_H
