#ifndef DARKWALLETIMPORTVIEW_H
#define DARKWALLETIMPORTVIEW_H

#include <QWidget>

namespace Ui {
class DarkWalletImportView;
}

class DarkWalletImportView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkWalletImportView(QWidget *parent = 0);
    ~DarkWalletImportView();

    void setFields(QString iAmount, QString iNotes);

private slots:
    void on_closeWindowButton_pressed();
    void on_cancelButton_pressed();
    void on_importButton_pressed();

signals:
    void addNotification (QString iTime, QString iNotificationText);

private:
    Ui::DarkWalletImportView *ui;
};

#endif // DARKWALLETIMPORTVIEW_H
