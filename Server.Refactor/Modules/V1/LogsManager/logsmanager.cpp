#include "logsmanager.h"
#include "main.h"
#include "modulelinker.h"
#include "app.h"

#include <QReadWriteLock>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef Q_OS_UNIX
#include <syslog.h>
#elif
#   define  syslog(...)
#endif

typedef enum OutputFormat {
    Human       = 0,
    Json        = 1
} OutputFormat;

static LogsManagerML            _gRegisterModuleLinker;
static QReadWriteLock           _gGlobalInstanceConstructionDestructionLock;
static OutputFormat             _gOutputFormat                                  = OutputFormat::Human;
static LogsManager *            _gGlobalInstance                                = nullptr;

LogsManagerML::LogsManagerML()
{
    ModuleLinker::registerModule(QStringLiteral("LogsManager-1"),LogsManagerML::creator,LogsManagerML::doInit,LogsManagerML::start,LogsManagerML::startInOwnThread);
}

void *LogsManagerML::creator(void *pointerToAppObject)
{
    App *               lApp            = reinterpret_cast<App *>(pointerToAppObject);
    auto                lConfig         = lApp->configuration();

    if( lConfig->contains(QStringLiteral("LogOutputFormat")) ) {
        QString     lOutputFormat = lConfig->toString(QStringLiteral("LogOutputFormat")).toLower();

        if( lOutputFormat == QStringLiteral("json") ) {
            _gOutputFormat = OutputFormat::Json;
        }
    }

    qInstallMessageHandler(LogsManager::messageHandler);

    QString             lLogsPath       = lConfig->toString(QStringLiteral("LogsPath"));
    LogsManager *       lLM             = new LogsManager{lLogsPath};
    lLM->mApp                           = lApp;
    return lLM;
}

bool LogsManagerML::doInit(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    LogsManager *   lLM = reinterpret_cast<LogsManager *>(pointerToThis);
    if( lLM ) {
        return lLM->doInit();
    }
    return false;
}

bool LogsManagerML::startInOwnThread()
{ return true; }

bool LogsManagerML::start(void *pointerToThis, void *pointerToAppObject)
{
    Q_UNUSED(pointerToAppObject)
    LogsManager *    lLM = reinterpret_cast<LogsManager *>(pointerToThis);
    if( lLM ) {
        QMetaObject::invokeMethod(lLM,"threadStarted",Qt::QueuedConnection);
        return true;
    }
    return false;
}

static QByteArray _createJsonOutput(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    QByteArray          lResult;

    switch (iType) {
    case QtDebugMsg:    lResult = "{\n\t\"Type\" : \"Debug\",";         break;
    case QtInfoMsg:     lResult = "{\n\t\"Type\" : \"Info\",";          break;
    case QtWarningMsg:  lResult = "{\n\t\"Type\" : \"Warning\",";       break;
    case QtCriticalMsg: lResult = "{\n\t\"Type\" : \"Critical\",";      break;
    case QtFatalMsg:    lResult = "{\n\t\"Type\" : \"Fatal\",";         break;
    }

    lResult = lResult.append(
                QStringLiteral("\n\t\"Timestamp\" : \"%7\",\n\t\"File\" : \"%1\",\n\t\"Function\" : \"%2\",\n\t\"Line\" : %3,\n\t\"Category\" : \"%4\",\n\t\"Version\" : %5,\n\t\"Message\" : \"%6\"\n}\n")
                .arg(iContext.file).arg(iContext.function).arg(iContext.line).arg(iContext.category == nullptr ? "(null)" : iContext.category).arg(iContext.version).arg(QDateTime::currentMSecsSinceEpoch())
                .arg(iMessage.toHtmlEscaped())
                );

    return lResult;
}

static QByteArray _createHumanOutput(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    QByteArray  lResult;

    switch (iType) {
    case QtDebugMsg:    lResult = QStringLiteral("%1 Debug ").arg(QDateTime::currentMSecsSinceEpoch()).toUtf8();     break;
    case QtInfoMsg:     lResult = QStringLiteral("%1 Info ").arg(QDateTime::currentMSecsSinceEpoch()).toUtf8();      break;
    case QtWarningMsg:  lResult = QStringLiteral("%1 Warning ").arg(QDateTime::currentMSecsSinceEpoch()).toUtf8();   break;
    case QtCriticalMsg: lResult = QStringLiteral("%1 Critical ").arg(QDateTime::currentMSecsSinceEpoch()).toUtf8();  break;
    case QtFatalMsg:    lResult = QStringLiteral("%1 Fatal ").arg(QDateTime::currentMSecsSinceEpoch()).toUtf8();     break;
    }

    lResult = lResult.append( QStringLiteral("[%1:%2, %3]: %4\n").arg(iContext.file).arg(iContext.line).arg(iContext.function).arg(iMessage) );

    return lResult;
}

static QByteArray _createOutput(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    switch(_gOutputFormat) {
    case OutputFormat::Json:    return _createJsonOutput(iType,iContext,iMessage);
    default:                    return _createHumanOutput(iType,iContext,iMessage);
    }
}

LogsManager::LogsManager(const QString iLogsPath, QObject * iParent)
    : LogsManagerInterface( iLogsPath, iParent )
    , mTimerInterval(kTimerIntervalInSeconds)
    , mMaxLogFileSizeInBytes(kMaxLogFileSizeInBytes)
    , mMaxLogFiles(kMaxLogFileCount)
    , mTimer(nullptr)
    , mLoggerHandle(stderr)
{

}

LogsManager::~LogsManager()
{
    _gGlobalInstanceConstructionDestructionLock.lockForWrite();
    _gGlobalInstance = nullptr;
    _gGlobalInstanceConstructionDestructionLock.unlock();
}

void LogsManager::messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    _gGlobalInstanceConstructionDestructionLock.lockForRead();
    if( _gGlobalInstance ) {
        _gGlobalInstance->_messageHandler(iType,iContext,iMessage);
        _gGlobalInstanceConstructionDestructionLock.unlock();
        return;
    }
    _gGlobalInstanceConstructionDestructionLock.unlock();

    // Fallback
    QByteArray      lOutput     = _createOutput(iType,iContext,iMessage);
    const char *    lCString    = lOutput.constData();
    size_t          lCStringLen = qstrlen(lCString);

    switch (iType) {
    case QtDebugMsg:
        // fprintf(stderr, "Debug [%s:%u, %s]: %s\n", iContext.file, iContext.line, iContext.function, lLocalMessage.constData());
        fwrite(lCString,lCStringLen,1,stderr);
        syslog(LOG_DEBUG, "%s", lCString );
        fflush(stderr);
        break;
    case QtInfoMsg:
        fwrite(lCString,lCStringLen,1,stderr);
        syslog(LOG_INFO, "%s", lCString );
        fflush(stderr);
        break;
    case QtWarningMsg:
        fwrite(lCString,lCStringLen,1,stderr);
        syslog(LOG_WARNING, "%s", lCString );
        fflush(stderr);
        break;
    case QtCriticalMsg:
        fwrite(lCString,lCStringLen,1,stderr);
        syslog(LOG_CRIT, "%s", lCString );
        fflush(stderr);
        break;
    case QtFatalMsg:
        fwrite(lCString,lCStringLen,1,stderr);
        syslog(LOG_EMERG, "%s", lCString );
        fflush(stderr);
        abort();
    }
}

bool LogsManager::doInit()
{
    QString lLogsPath = mApp->configuration()->toString(QStringLiteral("LogsPath"));
    QDir    lDir{lLogsPath};

    if( ! lDir.exists() ) {
        if( ! lDir.mkpath(lLogsPath) ) {
            qWarning() << "Unable to create logs storage path" << lLogsPath;
            return false;
        }
    }

    return true;
}

void LogsManager::threadStarted()
{
    _gGlobalInstanceConstructionDestructionLock.lockForWrite();
    _gGlobalInstance = this;

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

    _gGlobalInstanceConstructionDestructionLock.unlock();
}

void LogsManager::timerEventFired()
{
    _gGlobalInstanceConstructionDestructionLock.lockForRead();

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
    QDir    lPath{logsPath()};
    while( _countLogFiles() > mMaxLogFiles && lCount < 10) {
        lPath.remove(_findOldestFile());
        lCount++;
    }

    _gGlobalInstanceConstructionDestructionLock.unlock();
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
    QStringList lFiles = QDir(logsPath()).entryList(
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
    if( ! QDir(logsPath()).exists() ) {
        if( ! QDir(logsPath()).mkpath(logsPath()) )
            return QString();
    }

    QStringList lFiles = QDir(logsPath()).entryList(
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
    QString    lName = QString("%1%2%3").arg(logsPath()).arg(QDir::separator()).arg(iLogFilename);
    QByteArray lLocalFilename = lName.toLocal8Bit();
    return fopen(lLocalFilename,"a");
}

FILE *LogsManager::_startNewLogFile()
{
    QString    lName = QString("%1%2%3.log").arg(logsPath()).arg(QDir::separator()).arg(QDateTime::currentSecsSinceEpoch());
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
    QStringList lFiles = QDir(logsPath()).entryList(
                QStringList() << "*.log",
                QDir::Files
                );

    return lFiles.size();
}

void LogsManager::_messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage)
{
    QMutexLocker l{&mWriteLock};

    QByteArray      lOutput     = _createOutput(iType,iContext,iMessage);
    const char *    lCString    = lOutput.constData();
    size_t          lCStringLen = qstrlen(lCString);

    switch (iType) {
    case QtDebugMsg:
        fwrite(lCString,lCStringLen,1,mLoggerHandle);
        syslog(LOG_DEBUG, "%s", lCString );
        mBuffered = true;
        break;
    case QtInfoMsg:
        fwrite(lCString,lCStringLen,1,mLoggerHandle);
        syslog(LOG_INFO, "%s", lCString );
        mBuffered = true;
        break;
    case QtWarningMsg:
        fwrite(lCString,lCStringLen,1,mLoggerHandle);
        syslog(LOG_WARNING, "%s", lCString );
        fflush(mLoggerHandle);
        mBuffered = false;
        break;
    case QtCriticalMsg:
        fwrite(lCString,lCStringLen,1,mLoggerHandle);
        syslog(LOG_CRIT, "%s", lCString );
        fflush(mLoggerHandle);
        mBuffered = false;
        break;
    case QtFatalMsg:
        fwrite(lCString,lCStringLen,1,mLoggerHandle);
        syslog(LOG_EMERG, "%s", lCString );
        fflush(mLoggerHandle);
        mBuffered = false;
        abort();
    }
}
