#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "testeventmanager.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void addItem();
    void removeLastItem();

private:
    Ui::MainWindow *ui;
    TestEventManager *eventManager;
};

#endif // MAINWINDOW_H
