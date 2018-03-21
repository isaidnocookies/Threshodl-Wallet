#include "recordsmanager.h"

RecordsManager::RecordsManager(const QString &iRecordsPath, QObject * iParent)
    : QObject( iParent )
    , mRecordsPath(iRecordsPath)
{

}

void RecordsManager::threadStarted()
{

}
