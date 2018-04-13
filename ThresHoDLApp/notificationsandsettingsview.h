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

private:
    Ui::NotificationsAndSettingsView    *ui;
    UserAccount                         *mActiveUser;
};

#endif // NOTIFICATIONSANDSETTINGSVIEW_H
