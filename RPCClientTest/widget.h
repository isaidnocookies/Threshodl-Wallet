#ifndef WIDGET_H
#define WIDGET_H

#include "rpcconnection.h"

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_connectButton_pressed();
    void rpcOpenned();
    void rpcSslErrors(const QList<QSslError> iErrors);
    void rpcSocketError(QAbstractSocket::SocketError iError);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
