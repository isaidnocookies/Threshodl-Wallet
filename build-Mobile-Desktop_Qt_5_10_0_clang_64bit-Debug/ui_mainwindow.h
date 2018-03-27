/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *mainWidow;
    QWidget *gridLayoutWidget;
    QGridLayout *mainScreenBox;
    QLabel *counter;
    QHBoxLayout *actionsBox;
    QPushButton *deposit;
    QPushButton *withdraw;
    QTableView *itemList;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(440, 475);
        mainWidow = new QWidget(MainWindow);
        mainWidow->setObjectName(QStringLiteral("mainWidow"));
        gridLayoutWidget = new QWidget(mainWidow);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, -10, 441, 491));
        mainScreenBox = new QGridLayout(gridLayoutWidget);
        mainScreenBox->setSpacing(0);
        mainScreenBox->setContentsMargins(11, 11, 11, 11);
        mainScreenBox->setObjectName(QStringLiteral("mainScreenBox"));
        mainScreenBox->setContentsMargins(0, 0, 0, 0);
        counter = new QLabel(gridLayoutWidget);
        counter->setObjectName(QStringLiteral("counter"));
        QFont font;
        font.setPointSize(48);
        counter->setFont(font);
        counter->setFrameShape(QFrame::NoFrame);
        counter->setAlignment(Qt::AlignCenter);

        mainScreenBox->addWidget(counter, 0, 0, 1, 1);

        actionsBox = new QHBoxLayout();
        actionsBox->setSpacing(6);
        actionsBox->setObjectName(QStringLiteral("actionsBox"));
        deposit = new QPushButton(gridLayoutWidget);
        deposit->setObjectName(QStringLiteral("deposit"));

        actionsBox->addWidget(deposit);

        withdraw = new QPushButton(gridLayoutWidget);
        withdraw->setObjectName(QStringLiteral("withdraw"));

        actionsBox->addWidget(withdraw);


        mainScreenBox->addLayout(actionsBox, 1, 0, 1, 1);

        itemList = new QTableView(gridLayoutWidget);
        itemList->setObjectName(QStringLiteral("itemList"));

        mainScreenBox->addWidget(itemList, 2, 0, 1, 1);

        mainScreenBox->setRowStretch(0, 1);
        mainScreenBox->setRowStretch(2, 4);
        MainWindow->setCentralWidget(mainWidow);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        counter->setText(QApplication::translate("MainWindow", "0", nullptr));
        deposit->setText(QApplication::translate("MainWindow", "Deposit", nullptr));
        withdraw->setText(QApplication::translate("MainWindow", "Withdraw", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
