#ifndef BRIGHTSENDVIEW_H
#define BRIGHTSENDVIEW_H

#include <QWidget>

#include "globalsandstyle.h"
#include "qrreader.h"
#include "useraccount.h"

namespace Ui {
class BrightSendView;
}

class BrightSendView : public QWidget
{
    Q_OBJECT

public:
    explicit BrightSendView(QWidget *parent = 0);
    ~BrightSendView();
    void setActiveUser(UserAccount *iActiveUser);

private slots:
    void on_closePushButton_pressed();
    void on_scanQrPushButton_pressed();

    void on_sendTransactionButton_pressed();

public slots:
    void getCameraCode(QString iCode);

private:
    Ui::BrightSendView  *ui;
    QrReader            *mQrReaderView;
    UserAccount         *mActiveUser;
};

#endif // BRIGHTSENDVIEW_H
