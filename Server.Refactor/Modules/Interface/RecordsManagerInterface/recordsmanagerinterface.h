#ifndef RECORDSMANAGERINTERFACE_H
#define RECORDSMANAGERINTERFACE_H

#include <QObject>

class RecordsManagerInterface : public QObject
{
public:
    RecordsManagerInterface(QObject * iParent = nullptr);
    virtual ~RecordsManagerInterface();

signals:
    virtual void btcTestNetEstimateFeesChanged() = 0;
    virtual void btcMainNetEstimateFeesChanged() = 0;

public slots:
    virtual bool doInit()                                                                   = 0;
    virtual void threadStarted()                                                            = 0;
    virtual void handleDownloadedUrlData( const QString iUrl, const QByteArray iData )      = 0;
};

#endif // RECORDSMANAGERINTERFACE_H
