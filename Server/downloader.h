#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QString>
#include <QUrl>
#include <QMutex>
#include <QMap>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>

class DownloadEvent
{
public:
    QElapsedTimer                   Timer;
    QString                         UrlString;
    QUrl                            Url;
    bool                            Busy;
    QNetworkReply *                 Reply;
};

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *iParent = nullptr);

    QStringList urls() const;
    bool        isCurrentlyDownloading(const QString &iUrl) const;
    quint64     timeout() const;
    int         timerInterval() const;

signals:
    void    downloaded( const QString iUrl, const QByteArray iData );
    void    timedOut( const QString iUrl );
    void    failed( const QString iUrl );

public slots:
    void    threadStarted();
    void    timerEvent();
    void    setTimerInterval(int iTimerInterval);
    void    addUrl(const QString iUrl);

protected slots:
    void    networkReplyFinished( const QString iUrl, QNetworkReply * iSource );
    void    networkReplyError( const QString iUrl, QNetworkReply * iSource, QNetworkReply::NetworkError iErrorCode );

protected:
    void    restartTimer();

    QNetworkAccessManager *                         mNAM            = nullptr;
    int                                             mTimerInterval  = 5000;
    QTimer *                                        mTimer          = nullptr;
    quint64                                         mTimeOut        = 60000;
    QStringList                                     mURLs;

    mutable QMutex                                  mDownloadEventsLock;
    QMap< QString, QSharedPointer<DownloadEvent> >  mDownloadEvents;
};

#endif // DOWNLOADER_H
