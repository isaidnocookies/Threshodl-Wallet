#ifndef LOGSMANAGERV1_H
#define LOGSMANAGERV1_H

#include "../../Interface/LogsManagerInterface/logsmanagerinterface.h"

#include <QMutex>
#include <QTimer>

#define kTimerIntervalInSeconds     30
#define kMaxLogFileSizeInBytes      (1024 * 1024 * 1024) /* 1MB */
#define kMaxLogFileCount            1024

class App;
class LogsManagerML;
class LogsManager : public LogsManagerInterface
{
    Q_OBJECT
    friend class LogsManagerML;
public:
    explicit LogsManager(const QString iLogsPath, QObject *iParent = nullptr);

    static void messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage);   // private but needs to be exposed to external functions

signals:

public slots:
    bool doInit();
    void threadStarted();
    void timerEventFired();

protected:
    void _messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage) override;

    void _closeLogFile();
    QString _findOldestFile();
    QString _findNewestFile();
    FILE * _continueLogFile(const QString &iLogFilename);
    FILE * _startNewLogFile();
    qint64 _currentLogFileSize();
    int _countLogFiles();


    int         mTimerInterval;
    int         mMaxLogFileSizeInBytes;
    int         mMaxLogFiles;

    App *       mApp            = nullptr;
    QTimer *    mTimer;
    QMutex      mWriteLock;
    bool        mBuffered       = false;
    FILE *      mLoggerHandle;
};

class LogsManagerML
{
public:
    LogsManagerML();
    static void * creator(void * pointerToAppObject);                                      // Returns a pointer to a new object
    static bool doInit(void * pointerToThis, void * pointerToAppObject);                   // Returns true on DoInit success
    static bool startInOwnThread();                                                        // Returns true if should be created and started in own thread
    static bool start(void * pointerToThis, void * pointerToAppObject);                    // Returns true on Start success
};

#endif // LOGSMANAGERV1_H
