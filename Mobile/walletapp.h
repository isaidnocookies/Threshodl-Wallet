#ifndef WALLETAPP_H
#define WALLETAPP_H

#include <QObject>
#include <QString>

class WalletApp : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ getTitle WRITE setTitle NOTIFY titleChanged)
private:
    QString m_title;

signals:
    void titleChanged();

public:
    WalletApp();
    QString getTitle() const;
    void setTitle(const QString &title);

public slots:
    void listener(const QString &param1, int param2);
};

#endif // WALLETAPP_H
