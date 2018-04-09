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

    QCoreApplication::setOrganizationDomain(theDomain());
    QCoreApplication::setApplicationName(theApplicationName());
    QCoreApplication::setOrganizationName(theOrganization());
    mAccountSettings = new QSettings;

    ui->centralWidget->setGeometry(QApplication::desktop()->screenGeometry());

//    ui->brightButton->setStyleSheet(mainButtonStyle());
//    ui->darkButton->setStyleSheet(mainButtonStyle());

    ui->logoLabel->setText("");
    ui->logoLabel->setPixmap(QPixmap::fromImage(QImage(":/threshodl_logo")));
    ui->logoLabel->setPixmap(ui->logoLabel->pixmap()->scaledToWidth(QApplication::desktop()->screenGeometry().width(),Qt::SmoothTransformation));



    if ( mAccountSettings->contains(usernameSetting()) ) { // login
        QString lUsername = mAccountSettings->value(usernameSetting()).toString();
        QString lPublic = mAccountSettings->value(publicKeySetting()).toString();
        QString lPrivate = mAccountSettings->value(privateKeySetting()).toString();
        mCreateAccount = new CreateAccount;
        createAccountComplete(lUsername, lPublic, lPrivate);
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

    mAccountSettings->setValue(usernameSetting(), iUsername);
    mAccountSettings->setValue(publicKeySetting(), iPub);
    mAccountSettings->setValue(privateKeySetting(), iPriv);

    this->showMaximized();

    mCreateAccount->showNormal();
    mCreateAccount->hide();
    mCreateAccount->deleteLater();

    ui->welcomeLabel->setText(QString("Welcome, %1").arg(mActiveUser->getUsername()));
}

void MainWindow::makeMaximized()
{
    this->showMaximized();
}

void MainWindow::saveAddressInSettings(QString iEmail, QString iAddress)
{
    mAccountSettings->setValue(emailSetting(), iEmail);
    mAccountSettings->setValue(addressSetting(), iAddress);
}

void MainWindow::on_brightButton_pressed()
{
    mBrightWalletView = new BrightWallet;

    mBrightWalletView->setGeometry(QApplication::desktop()->screenGeometry());
    mBrightWalletView->showMaximized();
    mBrightWalletView->raise();
    mBrightWalletView->show();
}

void MainWindow::on_darkButton_pressed()
{
   mDarkWalletView = new DarkWallet;

   connect (mDarkWalletView, &DarkWallet::saveAddressSettings, this, &MainWindow::saveAddressInSettings);
   mDarkWalletView->setEmail(mAccountSettings->value(emailSetting()).toString());
   mDarkWalletView->setAddress(mAccountSettings->value(addressSetting()).toString());

   mDarkWalletView->setGeometry(QApplication::desktop()->screenGeometry());
   mDarkWalletView->showMaximized();
   mDarkWalletView->raise();
   mDarkWalletView->show();
}
