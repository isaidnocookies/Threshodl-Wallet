#ifndef DARKMICROWALLETVIEW_H
#define DARKMICROWALLETVIEW_H

#include <QWidget>

namespace Ui {
class DarkMicroWalletView;
}

class DarkMicroWalletView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkMicroWalletView(QWidget *parent = 0);
    ~DarkMicroWalletView();

private slots:
    void on_closeWindowButton_pressed();

private:
    Ui::DarkMicroWalletView *ui;
};

#endif // DARKMICROWALLETVIEW_H
