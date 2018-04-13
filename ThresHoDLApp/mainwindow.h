#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "createaccount.h"
#include "useraccount.h"
#include "brightwallet.h"
#include "darkwallet.h"
#include "notificationsandsettingsview.h"
#include "darkwalletimportview.h"

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

    Q_INVOKABLE void handleFileUrlReceived(const QUrl &url);

public slots:
    void createAccountComplete(QString iUsername, QByteArray iPriv, QByteArray iPub);
    void makeMaximized();
    void saveAddressInSettings(QString iEmail, QString iAddress);

private slots:
    void on_brightButton_pressed();
    void on_darkButton_pressed();
    void on_notificationPushButton_pressed();
    void brightToDarkCompleted(double lBrightAmount, QList<BitcoinWallet> iDarkWallets);

private:
    Ui::MainWindow                  *ui;
    CreateAccount                   *mCreateAccount;
    BrightWallet                    *mBrightWalletView;
    DarkWallet                      *mDarkWalletView;
    NotificationsAndSettingsView    *mNotificationView;
    DarkWalletImportView            *mDarkImportView;

    UserAccount                     *mActiveUser;

    void addNotificationToSettings (QDate iDate, QString iNotification);
    void setUI();
};

#endif // MAINWINDOW_H
