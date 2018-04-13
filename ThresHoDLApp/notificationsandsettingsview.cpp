#include "notificationsandsettingsview.h"
#include "ui_notificationsandsettingsview.h"

#include "globalsandstyle.h"

#include <QScroller>
#include <QDebug>

NotificationsAndSettingsView::NotificationsAndSettingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotificationsAndSettingsView)
{
    ui->setupUi(this);

    QScroller::grabGesture(ui->notificationTableWidget, QScroller::LeftMouseButtonGesture);
}

NotificationsAndSettingsView::~NotificationsAndSettingsView()
{
    delete ui;
}

void NotificationsAndSettingsView::setActiveUser(UserAccount &iUserAccount)
{
    mActiveUser = &iUserAccount;
    updateNotifications();
}

void NotificationsAndSettingsView::on_closeWindowButton_pressed()
{
    this->hide();
    this->deleteLater();
}

void NotificationsAndSettingsView::updateNotifications()
{
    ui->notificationTableWidget->clear();
    int lNotificationCount = mActiveUser->getNotifications().size();

    for (int i = 0; i < lNotificationCount; i+=2) {
        ui->notificationTableWidget->insertRow(0);
        ui->notificationTableWidget->setItem(0, 0, new QTableWidgetItem(mActiveUser->getNotifications().at(i)));
        ui->notificationTableWidget->setItem(0, 1, new QTableWidgetItem(mActiveUser->getNotifications().at(i+1)));
    }
}

void NotificationsAndSettingsView::on_notificationTableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(column)

    QString lDate = ui->notificationTableWidget->item(row, 0)->text();
    QString lNotification = ui->notificationTableWidget->item(row, 1)->text();

    ui->notificationLabel->setText(lNotification);
    ui->dateLabel->setText(lDate);
}