#ifndef RECORDSMANAGER_H
#define RECORDSMANAGER_H

#include <QObject>

class RecordsManager : public QObject
{
    Q_OBJECT
public:
    explicit RecordsManager(const QString &iRecordsPath, QObject *iParent = nullptr);

signals:

public slots:
    void threadStarted();

private:
    QString         mRecordsPath;
};

#endif // RECORDSMANAGER_H
