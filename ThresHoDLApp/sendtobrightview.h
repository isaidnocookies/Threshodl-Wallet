#ifndef SENDTOBRIGHTVIEW_H
#define SENDTOBRIGHTVIEW_H

#include <QWidget>

namespace Ui {
class SendToBrightView;
}

class SendToBrightView : public QWidget
{
    Q_OBJECT

public:
    explicit SendToBrightView(QWidget *parent = 0);
    ~SendToBrightView();

private slots:
    void on_closeButton_pressed();

    void on_convertButton_pressed();

private:
    Ui::SendToBrightView *ui;
};

#endif // SENDTOBRIGHTVIEW_H
