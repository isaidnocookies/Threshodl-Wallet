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

    mMainBalanceFont = ui->btcTotalLabel->font();

    ui->logoLabel->setText("");
    ui->logoLabel->setPixmap(QPixmap::fromImage(QImage(":/threshodl_logo")));
    ui->logoLabel->setPixmap(ui->logoLabel->pixmap()->scaledToWidth(QApplication::desktop()->screenGeometry().width(),Qt::SmoothTransformation));

    mActiveUser = new UserAccount;
    mPendingImport = "";

    connect (mActiveUser, &UserAccount::updateBalancesForMainWindow, this, &MainWindow::updateBalances);

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
        addNotificationToSettings(QDate::currentDate().toString(myDateFormat()), "Import Micro-Wallets Successful -- sort of");

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
            mPendingImport = lPackageArray;

            connect (mDarkImportView, &DarkWalletImportView::addNotification, this, &MainWindow::addNotificationToSettings);
            connect (mDarkImportView, &DarkWalletImportView::completeWalletImport, this, &MainWindow::completePendingImport);

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

void MainWindow::completePendingImport(bool iComplete)
{
    if (iComplete) {
        //import wallets
        QJsonDocument lDoc = QJsonDocument::fromJson(mPendingImport);
        QJsonObject lObject = lDoc.object();
        QJsonArray lArray = lObject["Wallets"].toArray();

        for (auto w : lArray) {
            if (mActiveUser->accountContainsWallet(BitcoinWallet(w.toVariant().toByteArray()).walletId())) {
                ui->warningLabel->setWordWrap(true);
                ui->warningLabel->setText("Wallet import was canceled. Wallet(s) already exist!");
                mPendingImport = "";
                return;
            }
        }

        for (auto w : lArray) {
            BitcoinWallet lNewWallet = BitcoinWallet(w.toVariant().toByteArray());
            mActiveUser->addMicroWallet(lNewWallet);
        }
    } else {
        mPendingImport = "";
    }
}

void MainWindow::on_brightButton_pressed()
{
    mBrightWalletView = new BrightWallet;

    connect (mBrightWalletView, &BrightWallet::brightToDarkCompletedSignal, this, &MainWindow::brightToDarkCompleted);

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

void MainWindow::brightToDarkCompleted(bool iSuccessful, double lBrightAmount, QList<QByteArray> iDarkWallets)
{
    double lDarkTotal = 0;
    for (auto dw : iDarkWallets) {
        lDarkTotal += BitcoinWallet(dw).value().toDouble();
    }

    addNotificationToSettings(QDate::currentDate().toString(myDateFormat()), QString("%1 was added to your Dark Wallet from your Bright Wallet!").arg(lDarkTotal));
}

void MainWindow::addNotificationToSettings(QString iDate, QString iNotification)
{
    QList<QVariant> lNotifications;
    mActiveUser->addNotification(iDate, iNotification);
}

void MainWindow::updateBalances(double iBrightBalance, double iDarkBalances)
{
    QString lTotalBalance = QString("%1").arg(QString::number(iBrightBalance + iDarkBalances, 'f'));

    lTotalBalance.remove( QRegExp("0+$") );
    if (lTotalBalance.at(lTotalBalance.size() - 1) == '.')
        lTotalBalance.append("00");

    bool    lFontFits = false;
    QFont   lFont = mMainBalanceFont;

    ui->brightBitcoinBalanceLabel->setText(QString("%1 BTC").arg(iBrightBalance));
    ui->darkBitcoinBalanceLabel->setText(QString("%1 BTC").arg(iDarkBalances));

    while (!lFontFits) {
        QFontMetrics lFm(lFont);
        QRect lBound = lFm.boundingRect(0,0, ui->btcTotalLabel->width(), ui->btcTotalLabel->height(), Qt::TextWordWrap | Qt::AlignCenter, lTotalBalance);

        if (lBound.width() <= QApplication::desktop()->screenGeometry().width())
            lFontFits = true;
        else
            lFont.setPointSize(lFont.pointSize() - 2);
    }

    ui->btcTotalLabel->setText(QString("%1").arg(lTotalBalance));
    ui->btcTotalLabel->setFont(lFont);
}

void MainWindow::setUI()
{
    ui->welcomeLabel->setText(QString("Welcome, %1").arg(mActiveUser->getUsername()));
    updateBalances(mActiveUser->getBrightBalance(), mActiveUser->getDarkBalance());
}
