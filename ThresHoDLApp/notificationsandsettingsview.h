#ifndef NOTIFICATIONSANDSETTINGSVIEW_H
#define NOTIFICATIONSANDSETTINGSVIEW_H

#include <QWidget>

#include "useraccount.h"

namespace Ui {
class NotificationsAndSettingsView;
}

class NotificationsAndSettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit NotificationsAndSettingsView(QWidget *parent = nullptr);
    ~NotificationsAndSettingsView();
    void setActiveUser(UserAccount &iUserAccount);

private slots:
    void on_closeWindowButton_pressed();
    void updateNotifications();
    void on_notificationTableWidget_cellClicked(int row, int column);
    void on_addPushButton_pressed();
    void on_deleteAccountButton_released();
    void on_backupPushButton_released();

    void on_resetBalancePushButton_pressed();

signals:
    void deleteAccountAndClearData();

private:
    Ui::NotificationsAndSettingsView    *ui;
    UserAccount                         *mActiveUser;

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();
};

#endif // NOTIFICATIONSANDSETTINGSVIEW_H
