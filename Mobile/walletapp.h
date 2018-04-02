#ifndef WALLETAPP_H
#define WALLETAPP_H

#include <QObject>
#include <QString>
#include <QList>
#include "wallet.h"
#include "bill.h"

class WalletApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int count READ getCount NOTIFY countChanged)
    Q_PROPERTY(QList<QObject*> wallets READ listWallets NOTIFY walletsChanged)
private:
    QString mTitle;
    QList<Wallet*> mWallets;

signals:
    void titleChanged();
    void countChanged();
    void walletsChanged();

public:
    WalletApp();
    QString getTitle() const;
    int getCount() const;
    void setTitle(const QString &title);
    QList<Wallet*> getWallets();
    QList<QObject*> listWallets();

public slots:
    void listener(const QString &param1, int param2);
};

#endif // WALLETAPP_H
