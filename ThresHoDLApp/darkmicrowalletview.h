#ifndef DARKMICROWALLETVIEW_H
#define DARKMICROWALLETVIEW_H

#include <QWidget>
#include <QMap>
#include <QScroller>

#include "bitcoinwallet.h"

namespace Ui {
class DarkMicroWalletView;
}

class DarkMicroWalletView : public QWidget
{
    Q_OBJECT

public:
    explicit DarkMicroWalletView(QWidget *parent = 0);
    ~DarkMicroWalletView();
    void setMicroWallets(QList<BitcoinWallet> iMicros);

private slots:
    void on_closeWindowButton_pressed();
    void on_microWalletTableWidget_cellPressed(int row, int column);

    void on_refreshPushButton_pressed();

private:
    Ui::DarkMicroWalletView     *ui;
    QMap <QString, QString>     mMicroWallets;
    QScroller*                  mScroller;
};

#endif // DARKMICROWALLETVIEW_H
