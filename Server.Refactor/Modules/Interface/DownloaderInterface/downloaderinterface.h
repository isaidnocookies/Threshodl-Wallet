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
    Q_OBJECT
protected:
    mutable QReadWriteLock      mUrlsLock;
    QList<QUrl>                 mUrls;

public:
    typedef QNetworkRequest (*UrlCallback_t)(const QUrl iUrl, void * iUserData);

    DownloaderInterface(QObject * iParent = nullptr);
    virtual ~DownloaderInterface();

    QList<QUrl> urls() const;

public slots:
    // Url callbacks are placed on allready added Urls or if the Url is not already added then it adds it as it is placed
    virtual void    addUrlCallback(const QUrl iUrl, UrlCallback_t iCallback, void * iUserData = nullptr)    = 0;
    virtual void    removeUrlCallback(const QUrl iUrl)                                                      = 0;

    virtual void    removeUrl(const QUrl iUrl);
    virtual void    addUrl(const QUrl iUrl);
    virtual void    setUrls(const QList<QUrl> iUrls);

signals:
    void    urlsChanged();
    void    downloaded( const QUrl iUrl, const QByteArray iData );
    void    timedOut( const QUrl iUrl );
    void    failed( const QUrl iUrl );
};

#endif // DOWNLOADERINTERFACE_H
