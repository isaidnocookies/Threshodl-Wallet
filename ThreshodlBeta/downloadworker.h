#ifndef DOWNLOADWORKER_H
#define DOWNLOADWORKER_H

#include <QObject>
#include <QNetworkAccessManager>

class DownloadWorker : public QObject
{
    Q_OBJECT
public:
    DownloadWorker();
    ~DownloadWorker();
signals:
    void finished();
    void error(QString err);
    void marketValueUpdated(QString oShortname, QString oValue);
    void allMarketValuesUpdated(QMap<QString, QString> oValues);

public slots:
    void startDownloading();
    void stopDownloading();

private:
    int                     mInterval;
    QNetworkAccessManager   *mNetworkManager;
    bool                    mDownloading;

    void downloadData();
};

#endif // DOWNLOADWORKER_H
