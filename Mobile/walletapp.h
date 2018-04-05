#ifndef WALLETAPP_H
#define WALLETAPP_H

#include <QObject>
#include <QString>
#include <QList>
#include "walletentity.h"
#include "billentity.h"
#include "sqliteinterface.h"

class WalletApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QList<QObject*> wallets READ listWallets NOTIFY walletsChanged)
private:
    QString mTitle;
    QList<WalletEntity*> mWallets;
    SQLiteInterface* sqliteinterface;

signals:
    void titleChanged();
    void walletsChanged();

public:
    WalletApp();
    QString getTitle() const;
    void setTitle(const QString &title);
    QList<QObject*> listWallets();

public slots:
    void listener(const QString &param1, int param2);
};

#endif // WALLETAPP_H
