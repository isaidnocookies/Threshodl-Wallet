#ifndef BRIGHTSENDVIEW_H
#define BRIGHTSENDVIEW_H

#include <QWidget>

namespace Ui {
class BrightSendView;
}

class BrightSendView : public QWidget
{
    Q_OBJECT

public:
    explicit BrightSendView(QWidget *parent = 0);
    ~BrightSendView();

private slots:
    void on_closePushButton_pressed();

private:
    Ui::BrightSendView *ui;
};

#endif // BRIGHTSENDVIEW_H
