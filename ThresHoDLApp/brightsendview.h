#ifndef BRIGHTSENDVIEW_H
#define BRIGHTSENDVIEW_H

#include <QWidget>

#include "globalsandstyle.h"
#include "qrreader.h"

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
    void on_scanQrPushButton_pressed();

public slots:
    void getCameraCode(QString iCode);

private:
    Ui::BrightSendView  *ui;
    QrReader            *mQrReaderView;
};

#endif // BRIGHTSENDVIEW_H
