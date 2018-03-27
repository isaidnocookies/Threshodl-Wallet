#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include "itemlistmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->eventManager = new TestEventManager();
    connect(this->ui->deposit, &QPushButton::clicked, this, &MainWindow::addItem);
    connect(this->ui->withdraw, &QPushButton::clicked, this, &MainWindow::removeLastItem);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addItem() {
    this->eventManager->addItem();
    this->ui->counter->setText(QString::number(this->eventManager->getTotal()));
    ItemListModel *ilm = new ItemListModel();
    this->ui->itemList->setModel(ilm);
}

void MainWindow::removeLastItem() {
    this->eventManager->removeLast();
    this->ui->counter->setText(QString::number(this->eventManager->getTotal()));
}
