#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "globalsandstyle.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QPropertyAnimation>
#include <QDesktopServices>
#include <QFileInfo>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setGeometry(QApplication::desktop()->screenGeometry());

    QDesktopServices::setUrlHandler("file", this, "handleFileUrlReceived");

    ui->logoLabel->setText("");
    ui->logoLabel->setPixmap(QPixmap::fromImage(QImage(":/threshodl_logo")));
    ui->logoLabel->setPixmap(ui->logoLabel->pixmap()->scaledToWidth(QApplication::desktop()->screenGeometry().width(),Qt::SmoothTransformation));

    mActiveUser = new UserAccount;

    if (mActiveUser->isNewAccount()) {
        mCreateAccount = new CreateAccount;
        connect(mCreateAccount, &CreateAccount::createUserAccount, this, &MainWindow::createAccountComplete);

        this->showNormal();

        mCreateAccount->showMaximized();
        mCreateAccount->show();
    } else {
        setUI();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleFileUrlReceived(const QUrl &url)
{
    QString incomingUrl = url.toString();
    if(incomingUrl.isEmpty()) {
        qWarning() << "setFileUrlReceived: we got an empty URL";
        return;
    }
    QString myUrl;
    if(incomingUrl.startsWith("file://")) {
        myUrl= incomingUrl.right(incomingUrl.length()-7);
        qDebug() << "QFile needs this URL: " << myUrl;
    } else {
        myUrl= incomingUrl;
    }

    QFileInfo fileInfo = QFileInfo(myUrl);
    if(fileInfo.exists()) {
        qDebug() << "Received! YA BITCHES!!";
        addNotificationToSettings(QDate::currentDate(), "Import Micro-Wallets Successful -- sort of");

        QFile lPackage(myUrl);
        QByteArray lPackageArray;

        if (!lPackage.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open package";
        } else {
            lPackageArray = lPackage.readAll();
            QJsonDocument lDoc = QJsonDocument::fromJson(lPackageArray);
            QJsonObject lObject = lDoc.object();
            QJsonArray lArray = lObject["Wallets"].toArray();

            mDarkImportView = new DarkWalletImportView;
            mDarkImportView->setFields(lObject["Amount"].toString(), lObject["Notes"].toString());

            mDarkImportView->show();
            mDarkImportView->showMaximized();

            qDebug() << lPackageArray;
        }

    } else {
        qDebug() << "setFileUrlReceived: FILE does NOT exist ";
    }
}

void MainWindow::createAccountComplete(QString iUsername, QByteArray iPriv, QByteArray iPub)
{
    mActiveUser->createNewAccount(iUsername, iPriv, iPub);

    mCreateAccount->hide();
    mCreateAccount->deleteLater();

    setUI();
}

void MainWindow::makeMaximized()
{
    this->showMaximized();
}

void MainWindow::saveAddressInSettings(QString iEmail, QString iAddress)
{
    mActiveUser->setAddresses(iEmail, iAddress);
}

void MainWindow::on_brightButton_pressed()
{
    mBrightWalletView = new BrightWallet;

    connect (mBrightWalletView, &BrightWallet::brightToDarkCompleted, this, &MainWindow::brightToDarkCompleted);

    mBrightWalletView->setAddress(mActiveUser->getBrightWallet().address());
    mBrightWalletView->setActiveUser(*mActiveUser);
    mBrightWalletView->show();
    mBrightWalletView->showMaximized();
}

void MainWindow::on_darkButton_pressed()
{
   mDarkWalletView = new DarkWallet;

   connect (mDarkWalletView, &DarkWallet::saveAddressSettings, this, &MainWindow::saveAddressInSettings);
   mDarkWalletView->setEmail(mActiveUser->getEmail());
   mDarkWalletView->setAddress(mActiveUser->getUsername());
   mDarkWalletView->setActiveUser(*mActiveUser);

   mDarkWalletView->showMaximized();
   mDarkWalletView->show();
}

void MainWindow::on_notificationPushButton_pressed()
{
    mNotificationView = new NotificationsAndSettingsView;

    mNotificationView->setActiveUser(*mActiveUser);
    mNotificationView->hide();
    mNotificationView->showMaximized();
}

void MainWindow::brightToDarkCompleted(double lBrightAmount, QList<BitcoinWallet> iDarkWallets)
{
    double lDarkTotal = 0;
    for (auto dw : iDarkWallets) {
        mActiveUser->addMicroWallet(dw);
        lDarkTotal += dw.value().toDouble();
    }

    ui->darkBitcoinBalanceLabel->setText(QString("%1 BTC").arg(mActiveUser->getDarkBalance()));

    mActiveUser->setBrightBalance(mActiveUser->getBrightBalance() - lBrightAmount);
    ui->brightBitcoinBalanceLabel->setText(QString("%1 BTC").arg(mActiveUser->getBrightBalance()));
    ui->btcTotalLabel->setText(QString("%1").arg(mActiveUser->getBrightBalance() + mActiveUser->getDarkBalance()));

    addNotificationToSettings(QDate::currentDate(), QString("%1 was added to your Dark Wallet from your Bright Wallet!").arg(lDarkTotal));
}

void MainWindow::addNotificationToSettings(QDate iDate, QString iNotification)
{
    QList<QVariant> lNotifications;

    mActiveUser->addNotification(iDate.toString(myDateFormat()), iNotification);
}

void MainWindow::setUI()
{
    ui->welcomeLabel->setText(QString("Welcome, %1").arg(mActiveUser->getUsername()));

    ui->btcTotalLabel->setText(QString("%1").arg(mActiveUser->getBrightBalance() + mActiveUser->getDarkBalance()));
    ui->brightBitcoinBalanceLabel->setText(QString("%1 BTC").arg(mActiveUser->getBrightBalance()));
    ui->darkBitcoinBalanceLabel->setText(QString("%1 BTC").arg(mActiveUser->getDarkBalance()));
}
