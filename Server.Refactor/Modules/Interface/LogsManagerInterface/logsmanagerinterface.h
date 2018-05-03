#ifndef LOGSMANAGERINTERFACE_H
#define LOGSMANAGERINTERFACE_H

#include <QString>
#include <QDebug>
#include <QObject>

class LogsManagerInterface : public QObject
{
    Q_OBJECT
protected:
    QString     mLogsPath;

    virtual void _messageHandler(QtMsgType iType, const QMessageLogContext &iContext, const QString &iMessage) = 0;

public:
    explicit LogsManagerInterface(const QString iLogsPath, QObject *iParent = nullptr) : QObject(iParent), mLogsPath(iLogsPath) { }
    virtual ~LogsManagerInterface() { }

    virtual QString logsPath() const { return mLogsPath; }
};

#endif // LOGSMANAGERINTERFACE_H
