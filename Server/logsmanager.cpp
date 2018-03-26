#include "logsmanager.h"
#include "app.h"

#include <QDir>
#include <QFileInfo>
#include <QDateTime>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

LogsManager::LogsManager(const QString iLogsPath, QObject * iParent)
    : QObject( iParent )
    , mTimerInterval(kTimerIntervalInSeconds)
    , mMaxLogFileSizeInBytes(kMaxLogFileSizeInBytes)
    , mMaxLogFiles(kMaxLogFileCount)
    , mLogsPath(iLogsPath)
    , mTimer(nullptr)
    , mLoggerHandle(stderr)
{

}

void LogsManager::messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    if( gApp && gApp->logsManager() ) {
        gApp->logsManager()->_messageHandler(iType,iContext,iMessage);
        return;
    }

    // Fallback
    QByteArray lLocalMessage = iMessage.toLocal8Bit();

    switch (iType) {
    case QtDebugMsg:
        fprintf(stderr, "Debug [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(stderr);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(stderr);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(stderr);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(stderr);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(stderr);
        abort();
    }
}

void LogsManager::threadStarted()
{   
    if( mTimer == nullptr ) {
        mTimer = new QTimer{this};
        connect( mTimer, &QTimer::timeout, this, &LogsManager::timerEventFired );
        mTimer->setInterval( mTimerInterval * 1000 );
        mTimer->setSingleShot(false);
        mTimer->start();
    }

    if( mLoggerHandle != stderr ) {
        fclose(mLoggerHandle);
        mLoggerHandle = stderr;
    }

    QString lFilename       = _findNewestFile();
    bool    lStartNewFile   = (lFilename.isEmpty() || QFileInfo(lFilename).size() >= mMaxLogFileSizeInBytes);
    FILE *  lNewHandle      = nullptr;

    if( lStartNewFile ) {
        lNewHandle = _startNewLogFile();
    } else {
        lNewHandle = _continueLogFile(lFilename);
    }

    if( lNewHandle ) {
        mWriteLock.lock();
        mLoggerHandle = lNewHandle;
        mWriteLock.unlock();
    } else {
        qWarning() << "Could not open log file.";
    }
}

void LogsManager::timerEventFired()
{
    if( mBuffered ) {
        mWriteLock.lock();
        if( mBuffered ) {
            mBuffered = false;
            if(mLoggerHandle)
                fflush(mLoggerHandle);
        }
        mWriteLock.unlock();
    }

    // Check log file size, if its too large then close it and move to next file
    if( _currentLogFileSize() >= mMaxLogFileSizeInBytes ) {
        mWriteLock.lock();
        _closeLogFile();
        _startNewLogFile();
        mWriteLock.unlock();
    }

    // Scan folder for files to clean up
    int     lCount = 0;
    QDir    lPath{mLogsPath};
    while( _countLogFiles() > mMaxLogFiles && lCount < 10) {
        lPath.remove(_findOldestFile());
        lCount++;
    }
}

void LogsManager::_closeLogFile()
{
    if( mLoggerHandle != stderr ) {
        fclose(mLoggerHandle);
        mLoggerHandle = stderr;
    }
}

QString LogsManager::_findOldestFile()
{
    QStringList lFiles = QDir(mLogsPath).entryList(
                QStringList() << "*.log",
                QDir::Files,
                QDir::Time | QDir::Reversed
                );

    if( ! lFiles.isEmpty() ) {
        return lFiles.first();
    }

    return QString();
}

QString LogsManager::_findNewestFile()
{
    if( ! QDir(mLogsPath).exists() ) {
        if( ! QDir(mLogsPath).mkpath(mLogsPath) )
            return QString();
    }

    QStringList lFiles = QDir(mLogsPath).entryList(
                QStringList() << "*.log",
                QDir::Files,
                QDir::Time | QDir::Reversed
                );

    if( ! lFiles.isEmpty() ) {
        return lFiles.last();
    }

    return QString();
}

FILE *LogsManager::_continueLogFile(const QString &iLogFilename)
{
    QString    lName = QString("%1%2%3").arg(mLogsPath).arg(QDir::separator()).arg(iLogFilename);
    QByteArray lLocalFilename = lName.toLocal8Bit();
    return fopen(lLocalFilename,"a");
}

FILE *LogsManager::_startNewLogFile()
{
    QString    lName = QString("%1%2%3.log").arg(mLogsPath).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch());
    QByteArray lLocalFilename = lName.toLocal8Bit();
    return fopen(lLocalFilename,"w");
}

qint64 LogsManager::_currentLogFileSize()
{
    struct stat lStat;

    if( mLoggerHandle == stderr || 0 != fstat(fileno(mLoggerHandle), &lStat) )
        return 0;

    return lStat.st_size;
}

int LogsManager::_countLogFiles()
{
    QStringList lFiles = QDir(mLogsPath).entryList(
                QStringList() << "*.log",
                QDir::Files
                );

    return lFiles.size();
}

void LogsManager::_messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    QByteArray lLocalMessage = iMessage.toLocal8Bit();

    QMutexLocker l{&mWriteLock};

    switch (iType) {
    case QtDebugMsg:
        fprintf(mLoggerHandle, "Debug [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        mBuffered = true;
        break;
    case QtInfoMsg:
        fprintf(mLoggerHandle, "Info [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        mBuffered = true;
        break;
    case QtWarningMsg:
        fprintf(mLoggerHandle, "Warning [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(mLoggerHandle);
        mBuffered = false;
        break;
    case QtCriticalMsg:
        fprintf(mLoggerHandle, "Critical [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(mLoggerHandle);
        mBuffered = false;
        break;
    case QtFatalMsg:
        fprintf(mLoggerHandle, "Fatal [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fflush(mLoggerHandle);
        mBuffered = false;
        abort();
    }
}
