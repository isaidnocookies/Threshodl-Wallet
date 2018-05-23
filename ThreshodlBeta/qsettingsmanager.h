#ifndef QSETTINGSMANAGER_H
#define QSETTINGSMANAGER_H

#include <QObject>
#include <QSettings>

class QSettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit QSettingsManager(QObject *parent = nullptr);

    bool userAccountExists();
    void saveUsername(QString iUsername);
    void savePublicAndPrivateKeys(QByteArray iPublicKey, QByteArray iPrivateKey);
    void usernameAndKeys(QString &oUsername, QByteArray &oPublicKey, QByteArray &oPrivateKey);

protected:
    QSettings *mAccountData;
};

#endif // QSETTINGSMANAGER_H
