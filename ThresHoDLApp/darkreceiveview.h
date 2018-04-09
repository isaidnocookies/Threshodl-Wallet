#ifndef DARKRECEIVEVIEW_H
#define DARKRECEIVEVIEW_H

#include <QWidget>

namespace Ui {
class DarkReceiveView;
}

class DarkReceiveView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkReceiveView(QWidget *parent = 0);
    ~DarkReceiveView();

    void setAddresses(QString iEmail, QString iAddress);

private:
    void createQrCode();

private slots:
    void on_closeWindowButton_pressed();
    void on_updateQRCodePushButton_pressed();

signals:
    void saveAddresses(QString iEmail, QString iAddress);

private:
    Ui::DarkReceiveView     *ui;
    QImage                  *mQrImage;
    QString                 mEmailAddress;
    QString                 mThreshodlAddress;
};

#endif // DARKRECEIVEVIEW_H
