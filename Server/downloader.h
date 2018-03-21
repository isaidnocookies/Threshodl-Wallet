#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkAccessManager>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *iParent = nullptr);

signals:

public slots:
    void threadStarted();
    void timerEvent();

private:
    QNetworkAccessManager *     mNAM    = nullptr;
};

#endif // DOWNLOADER_H
