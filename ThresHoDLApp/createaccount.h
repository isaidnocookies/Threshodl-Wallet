#ifndef CREATEACCOUNT_H
#define CREATEACCOUNT_H

#include <QWidget>
#include <QDebug>

namespace Ui {
class CreateAccount;
}

class CreateAccount : public QWidget
{
    Q_OBJECT

public:
    explicit CreateAccount(QWidget *parent = nullptr);
    ~CreateAccount();

private slots:
    void on_createAccountButton_pressed();

signals:
    void createUserAccount(QString oUsername, QString oPriv, QString oPub);

private:
    Ui::CreateAccount *ui;
};

#endif // CREATEACCOUNT_H
