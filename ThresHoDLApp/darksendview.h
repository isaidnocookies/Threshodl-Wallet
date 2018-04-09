#ifndef DARKSENDVIEW_H
#define DARKSENDVIEW_H

#include <QWidget>

namespace Ui {
class DarkSendView;
}

class DarkSendView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkSendView(QWidget *parent = nullptr);
    void setEmail(QString iEmail);
    ~DarkSendView();

private slots:
    void on_closePushButton_pressed();
    void on_scanQrButton_pressed();
    void on_sendTransactionButton_pressed();

private:
    Ui::DarkSendView    *ui;
};

#endif // DARKSENDVIEW_H
