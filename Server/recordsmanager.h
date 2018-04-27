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

    QByteArray  lastDataBTCUSD() const;
    QByteArray  lastDataETHUSD() const;
    QByteArray  lastDataETHBTC() const;
    QByteArray  testNetEstimateFee() const;

signals:

public slots:
    void threadStarted();

    void handleDownloadedUrlData( const QString iUrl, const QByteArray iData );

protected:
    void    saveDataBTCTestNetBlockChainStats(const QString iSource, const QByteArray iData);
    void    saveDataBTCUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHBTC(const QString iSource, const QByteArray iData);

private:
    void _recordFileAndStoreLocalVariable(const QString iPath, const QString iFilename, const QByteArray iData, QByteArray &oLocalVariable);

    QString         mRecordsPath;
    QByteArray      mTestNetEstimateFee;
    QByteArray      mDataBTCUSD;
    QByteArray      mDataETHUSD;
    QByteArray      mDataETHBTC;
};

#endif // RECORDSMANAGER_H
