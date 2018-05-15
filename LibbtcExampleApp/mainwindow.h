#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "blockchaintool.h"
#include "bitcoinwallet.h"
#include "globalsandconstants.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_createWalletPushButton_released();
    void on_savedWalletTableWidget_cellClicked(int row, int column);
    void on_deletePushButton_released();
    void on_decodeTransactionPushButton_released();
    void on_createRawTransactionPushButton_released();
    void on_addUtxoPushButton_released();
    void on_removeSelectedUtxoPushButton_4_released();
    void on_addOutputPushButton_2_released();
    void on_removeOutputPushButton_3_released();
    void on_signTransactionPushButton_released();
    void on_addPrivateKeyPushButton_released();
    void on_removePrivateKeyPushButton_2_released();
    void openBlockExplorerAction();
    void openTestFaucetManu();
    void openTestFaucetTest3();

    void on_forNodeGetUtxoPushButton_released();

    void on_addOutputNodeCreateRawPushButton_2_released();

    void on_deleteOutputNodeCreateRawPushButton_3_released();

    void on_createRawTransactionFromNodePushButton_released();

    void on_fromNodeUtxoTableWidget_itemSelectionChanged();

private:
    Ui::MainWindow          *ui;
    QList<BitcoinWallet>    mWallets;
    QSettings               *mAppSettings;

    bool saveWalletToSettings (BitcoinWallet iNewWallet);
    bool loadFromSettings();

    void saveInputToSettings(QString iUtxo, QString iVout);
    void saveOutputToSettings(QString iAddress, QString iAmount);
    void savePrivateKeysToSettings(QStringList iPrivateKeys);
    void saveRawTransactionForSigningToSettings(QString iTransaction);

    void startProgressBarAndDisable();
    void stopProgressBarAndEnable();
};

#endif // MAINWINDOW_H
