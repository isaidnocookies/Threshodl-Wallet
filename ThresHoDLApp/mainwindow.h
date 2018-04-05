#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "createaccount.h"
#include "useraccount.h"
#include "brightwallet.h"
#include "darkwallet.h"

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void createAccountComplete(QString iUsername, QString iPriv, QString iPub);
    void makeMaximized();

private slots:
    void on_brightButton_pressed();
    void on_darkButton_pressed();

private:
    Ui::MainWindow  *ui;
    CreateAccount   *mCreateAccount;
    UserAccount     *mActiveUser;
    BrightWallet    *mBrightWalletView;
    DarkWallet      *mDarkWalletView;

};

#endif // MAINWINDOW_H
