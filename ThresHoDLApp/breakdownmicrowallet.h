#ifndef BREAKDOWNMICROWALLET_H
#define BREAKDOWNMICROWALLET_H

#include "core.h"
#include "globalsandstyle.h"
#include "useraccount.h"

#include <QDir>
#include <QList>
#include <QFile>
#include <QDebug>
#include <QSslKey>
#include <QObject>
#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QSslConfiguration>
#include <QSslCertificate>

class BreakdownMicroWallet : public QObject
{
    Q_OBJECT

    // enum for commands
    enum class ActionCommands {
        None            = 0x1,
        CompleteWallet  = 0x2,
        BreakdownWallet = 0x3,
        Complete        = 0x4
    };

    typedef enum ActionCommands ActionCommands;

public:
    explicit BreakdownMicroWallet(QObject *parent = nullptr);
    void getMicroWallets(BitcoinWallet iWallet);
    void setActiveUser(UserAccount *iActiveUser);

public slots:
    void connectedToServer();
    void disconnectedFromServer();
    void failedToSendMessage();
    void sentMessage();
    void receivedMessage();
    void socketError(QAbstractSocket::SocketError iError);
    void sslErrors(const QList<QSslError> iErrors);

signals:
    void newMicroWallets(bool success);
    void completeMicroWallet(bool oSuccess);
    void microWalletBreakdownComplete(bool oSuccess);

private:
    QString             mTransactionID;
    WCPConnection       *mConnection;
    QSslConfiguration   mSslConfiguration;
    BitcoinWallet       mBitcoinWalletToComplete;
    bool                mBitcoinWasCompleted;
    ActionCommands      mCurrentAction;
    UserAccount         *mActiveUser;
    int                 mConnectionAttempts;

    void completeWallet (QMap<QString, QByteArray> iData);
    void getMicroWalletsFromReply (QList<QByteArray> iData);
};

#endif // BREAKDOWNMICROWALLET_H
