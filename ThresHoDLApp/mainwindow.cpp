#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "globalsandstyle.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->setGeometry(QApplication::desktop()->screenGeometry());

    ui->brightButton->setStyleSheet(mainButtonStyle());
    ui->darkButton->setStyleSheet(mainButtonStyle());

    if (false) { // login
        mCreateAccount = new CreateAccount;
        createAccountComplete("chris.threshodl", "234567876trfdcv", "76tfghji876tfghjiuyt");
    } else { // create user
        this->showNormal();

        mCreateAccount = new CreateAccount;
        connect(mCreateAccount, &CreateAccount::createUserAccount, this, &MainWindow::createAccountComplete);

        mCreateAccount->showMaximized();
        mCreateAccount->show();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createAccountComplete(QString iUsername, QString iPriv, QString iPub)
{
    mActiveUser = new UserAccount(iUsername);
    mActiveUser->setKeys(iPub, iPriv);

    this->showMaximized();

    mCreateAccount->showNormal();
    mCreateAccount->hide();
    mCreateAccount->deleteLater();

    ui->welcomeLabel->setText(QString("Welcome %1").arg(mActiveUser->getUsername()));
}

void MainWindow::makeMaximized()
{
    this->showMaximized();
}

void MainWindow::on_brightButton_pressed()
{
    mBrightWalletView = new BrightWallet;
    connect(mBrightWalletView, &BrightWallet::makeDashboardMaximized, this, &MainWindow::makeMaximized);

    mBrightWalletView->setGeometry(QApplication::desktop()->screenGeometry());
    mBrightWalletView->showMaximized();
    mBrightWalletView->raise();
    mBrightWalletView->show();
}

void MainWindow::on_darkButton_pressed()
{
   mDarkWalletView = new DarkWallet;

   mDarkWalletView->setGeometry(QApplication::desktop()->screenGeometry());
   mDarkWalletView->showMaximized();
   mDarkWalletView->raise();
   mDarkWalletView->show();
}
