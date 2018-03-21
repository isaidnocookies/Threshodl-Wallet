#ifndef LOGSMANAGER_H
#define LOGSMANAGER_H

#include <QString>
#include <QMutex>
#include <QDebug>
#include <QObject>
#include <QTimer>

#define kTimerIntervalInSeconds     60
#define kMaxLogFileSizeInBytes      (1024 * 1024 * 1024) /* 1MB */
#define kMaxLogFileCount            1024

class LogsManager : public QObject
{
    Q_OBJECT
public:
    explicit LogsManager(const QString iLogsPath, QObject *iParent = nullptr);

    static void messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage);   // private but needs to be exposed to external functions

signals:

public slots:
    void threadStarted();
    void timerEventFired();

private:
    void _closeLogFile();
    QString _findOldestFile();
    QString _findNewestFile();
    FILE * _continueLogFile(const QString &iLogFilename);
    FILE * _startNewLogFile();
    qint64 _currentLogFileSize();
    int _countLogFiles();

    void _messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage);

    int         mTimerInterval;
    int         mMaxLogFileSizeInBytes;
    int         mMaxLogFiles;

    QString     mLogsPath;
    QTimer *    mTimer;
    QMutex      mWriteLock;
    bool        mBuffered       = false;
    FILE *      mLoggerHandle;
};

#endif // LOGSMANAGER_H
