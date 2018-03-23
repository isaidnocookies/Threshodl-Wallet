#ifndef RECORDSMANAGER_H
#define RECORDSMANAGER_H

#include <QByteArray>
#include <QString>
#include <QObject>

class RecordsManager : public QObject
{
    Q_OBJECT
public:
    explicit RecordsManager(const QString &iRecordsPath, QObject *iParent = nullptr);

    QByteArray lastDataBTCUSD() const;
    QByteArray lastDataETHUSD() const;
    QByteArray lastDataETHBTC() const;

signals:

public slots:
    void threadStarted();

    void handleDownloadedUrlData( const QString iUrl, const QByteArray iData );

protected:
    void    saveDataBTCUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHBTC(const QString iSource, const QByteArray iData);

private:
    QString         mRecordsPath;
    QByteArray      mDataBTCUSD;
    QByteArray      mDataETHUSD;
    QByteArray      mDataETHBTC;
};

#endif // RECORDSMANAGER_H
