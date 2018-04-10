#ifndef DARKSENDCONFIRM_H
#define DARKSENDCONFIRM_H

#include <QWidget>

namespace Ui {
class DarkSendConfirm;
}

class DarkSendConfirm : public QWidget
{
    Q_OBJECT

public:
    explicit DarkSendConfirm(QWidget *parent = 0);
    ~DarkSendConfirm();

private slots:
    void on_closeWindowButton_pressed();
    void on_cancelPushButton_pressed();
    void on_confirmSendPushButton_pressed();

signals:
    void sendConfirmation(bool iSuccess);

private:
    Ui::DarkSendConfirm *ui;
};

#endif // DARKSENDCONFIRM_H
