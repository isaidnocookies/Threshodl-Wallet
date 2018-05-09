#ifndef RECORDSMANAGERV1_H
#define RECORDSMANAGERV1_H

#include "../../Interface/RecordsManagerInterface/recordsmanagerinterface.h"

#include <QByteArray>
#include <QString>
#include <QObject>

class App;
class RecordsManagerML;
class RecordsManager : public RecordsManagerInterface
{
    Q_OBJECT
    friend class RecordsManagerML;
public:
    explicit RecordsManager(const QString &iRecordsPath, QObject *iParent = nullptr);

    QByteArray  lastDataBTCUSD() const;
    QByteArray  lastDataETHUSD() const;
    QByteArray  lastDataETHBTC() const;
    QByteArray  testNetEstimateFee() const;

signals:
    void btcTestNetEstimateFeesChanged() override;
    void btcMainNetEstimateFeesChanged() override;

public slots:
    bool doInit() override;
    void threadStarted() override;
    void handleDownloadedUrlData( const QString iUrl, const QByteArray iData ) override;
    void recordFee(const QString iCryptoCurrency, const QString iType, const QString iFee) override;

protected:
    void    saveDataBTCTestNetBlockChainStats(const QString iSource, const QByteArray iData);
    void    saveDataBTCUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHUSD(const QString iSource, const QByteArray iData);
    void    saveDataETHBTC(const QString iSource, const QByteArray iData);

private:
    void _recordFileAndStoreLocalVariable(const QString iPath, const QString iFilename, const QByteArray iData, QByteArray &oLocalVariable);

    App *           mApp;
    QString         mRecordsPath;
    QByteArray      mTestNetEstimateFee;
    QByteArray      mDataBTCUSD;
    QByteArray      mDataETHUSD;
    QByteArray      mDataETHBTC;
};

class RecordsManagerML
{
public:
    RecordsManagerML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // RECORDSMANAGERV1_H
