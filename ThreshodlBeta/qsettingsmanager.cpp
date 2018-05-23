#include "qsettingsmanager.h"
#include "globalsandconstants.h"

#include <QCoreApplication>

QSettingsManager::QSettingsManager(QObject *parent) : QObject(parent)
{
    QCoreApplication::setOrganizationDomain(AppInfo::appDomain());
    QCoreApplication::setApplicationName(AppInfo::appName());
    QCoreApplication::setOrganizationName(AppInfo::appOrganization());

    mAccountData = new QSettings;
}

bool QSettingsManager::userAccountExists()
{
    if (mAccountData->contains(DataKeys::usernameDataKey())) {
        return true;
    } else {
        return false;
    }
}

void QSettingsManager::saveUsername(QString iUsername)
{
    mAccountData->setValue(DataKeys::usernameDataKey(), iUsername);
    mAccountData->sync();
}

void QSettingsManager::savePublicAndPrivateKeys(QByteArray iPublicKey, QByteArray iPrivateKey)
{
    mAccountData->setValue(DataKeys::publicKeyDataKey(), iPublicKey);
    mAccountData->setValue(DataKeys::privateKeyDataKey(), iPrivateKey);

    mAccountData->sync();
}

void QSettingsManager::usernameAndKeys(QString &oUsername, QByteArray &oPublicKey, QByteArray &oPrivateKey)
{
    oUsername = mAccountData->value(DataKeys::usernameDataKey()).toString();
    oPublicKey = mAccountData->value(DataKeys::publicKeyDataKey()).toByteArray();
    oPrivateKey = mAccountData->value(DataKeys::privateKeyDataKey()).toByteArray();
}
