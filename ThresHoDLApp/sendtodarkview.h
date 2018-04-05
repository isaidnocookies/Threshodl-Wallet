#ifndef SENDTODARKVIEW_H
#define SENDTODARKVIEW_H

#include <QWidget>

namespace Ui {
class SendToDarkView;
}

class SendToDarkView : public QWidget
{
    Q_OBJECT

public:
    explicit SendToDarkView(QWidget *parent = 0);
    ~SendToDarkView();

private slots:
    void on_closeButton_pressed();

    void on_convertButton_pressed();

private:
    Ui::SendToDarkView *ui;
};

#endif // SENDTODARKVIEW_H
