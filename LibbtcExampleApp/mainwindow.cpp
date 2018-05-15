#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qstringmath.h"

#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    mAppSettings = new QSettings(theDomain(),theApplicationName());
    loadFromSettings();

    connect (ui->actionOpen_BlockExplorer, &QAction::triggered, this, &MainWindow::openBlockExplorerAction);
    connect (ui->actionOpen_Testnet_Faucet_Manu, &QAction::triggered, this, &MainWindow::openTestFaucetManu);
    connect (ui->actionOpen_Testnet_Faucet_Test3, &QAction::triggered, this, &MainWindow::openTestFaucetTest3);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createWalletPushButton_released()
{
    BitcoinWallet lNewWallet = BlockchainTool::createNewBitcoinWallet(currentChain());
    QString lOutput;

    if ( !ui->walletNameLineEdit->text().isEmpty()) {
        lNewWallet.setOwner(ui->walletNameLineEdit->text());
    }

    saveWalletToSettings(lNewWallet);

    lOutput = "Wallet Creation Complete\n\n";
    lOutput.append(QString("New Wallet Address:\n  %1\n\n").arg(QString(lNewWallet.address())));
    lOutput.append(QString("New Wallet Wif:\n  %1\n\n").arg(QString(lNewWallet.wif())));
    lOutput.append(QString("New Wallet Private Key:\n  %1\n\n").arg(QString(lNewWallet.privateKey())));
    lOutput.append(QString("New Wallet Public Key:\n  %1\n\n").arg(QString(lNewWallet.publicKey())));

    ui->createWalletOutputLabel->setText(lOutput);
}

bool MainWindow::saveWalletToSettings(BitcoinWallet iNewWallet)
{
    QList<QVariant> lSavedWallets;

    lSavedWallets = mAppSettings->value(walletSetting()).toList();
    lSavedWallets.append(iNewWallet.toData());
    mAppSettings->setValue(walletSetting(), lSavedWallets);

    ui->savedWalletTableWidget->insertRow(0);
    ui->savedWalletTableWidget->setItem(0,0, new QTableWidgetItem(iNewWallet.owner()));
    ui->savedWalletTableWidget->setItem(0, 1, new QTableWidgetItem(QString(iNewWallet.address())));
    ui->savedWalletTableWidget->item(0,0)->setData(Qt::UserRole, iNewWallet.toData());

    mAppSettings->sync();
    return true;
}

bool MainWindow::loadFromSettings()
{
    QList<QVariant> lWalletData;
    QList<QVariant> lInputUtxos;
    QList<QVariant> lInputVouts;
    QList<QVariant> lOutputAddresss;
    QList<QVariant> lOutputAmounts;

    if (mAppSettings->contains(walletSetting())) {
        lWalletData = mAppSettings->value(walletSetting()).toList();

        for (auto wd : lWalletData) {
            BitcoinWallet lWallet(wd.toByteArray());
            mWallets.append(lWallet);

            ui->savedWalletTableWidget->insertRow(0);
            ui->savedWalletTableWidget->setItem(0,0, new QTableWidgetItem(lWallet.owner()));
            ui->savedWalletTableWidget->setItem(0, 1, new QTableWidgetItem(QString(lWallet.address())));
            ui->savedWalletTableWidget->item(0,0)->setData(Qt::UserRole, lWallet.toData());
        }
    }

    if (mAppSettings->contains(inputUtxosSetting())) {
        lInputUtxos = mAppSettings->value(inputUtxosSetting()).toList();
        lInputVouts = mAppSettings->value(inputVoutSetting()).toList();

        for (int i = 0; i < lInputUtxos.size(); i++) {
            ui->inputsTableWidget->insertRow(0);
            ui->inputsTableWidget->setItem(0,0, new QTableWidgetItem(lInputUtxos.at(i).toString()));
            ui->inputsTableWidget->setItem(0,1, new QTableWidgetItem(lInputVouts.at(i).toString()));
        }
    }

    if (mAppSettings->contains(outputAddressSetting())) {
        lOutputAddresss = mAppSettings->value(outputAddressSetting()).toList();
        lOutputAmounts = mAppSettings->value(outputAmountSetting()).toList();

        for (int i = 0; i < lOutputAddresss.size(); i++) {
            ui->outputsTableWidget_2->insertRow(0);
            ui->outputsTableWidget_2->setItem(0,0, new QTableWidgetItem(lOutputAddresss.at(i).toString()));
            ui->outputsTableWidget_2->setItem(0,1, new QTableWidgetItem(lOutputAmounts.at(i).toString()));
        }
    }

    if (mAppSettings->contains(currentRawHex())) {
        ui->rawTransactionPlainTextEdit->document()->setPlainText(mAppSettings->value(currentRawHex()).toString());
    }

    if (mAppSettings->contains(privateKeySetting())) {
        QStringList lPrivateKeys = mAppSettings->value(privateKeySetting()).toStringList();

        for (auto pk : lPrivateKeys) {
            ui->privateKeyForSigningTableWidget->insertRow(0);
            ui->privateKeyForSigningTableWidget->setItem(0, 0, new QTableWidgetItem(pk));
        }
    }

    if (mAppSettings->contains(rawTransactionForSigningSetting())) {
        ui->rawTransactionForSigningPlainTextEdit->document()->setPlainText(mAppSettings->value(rawTransactionForSigningSetting()).toString());
    }

    return true;
}

void MainWindow::saveInputToSettings(QString iUtxo, QString iVout)
{
    QList<QVariant> lUtxos = mAppSettings->value(inputUtxosSetting()).toList();
    QList<QVariant> lVout = mAppSettings->value(inputVoutSetting()).toList();

    lUtxos.append(iUtxo);
    lVout.append(iVout);

    mAppSettings->setValue(inputUtxosSetting(), lUtxos);
    mAppSettings->setValue(inputVoutSetting(), lVout);

    mAppSettings->sync();
}

void MainWindow::saveOutputToSettings(QString iAddress, QString iAmount)
{
    QList<QVariant> lAddress = mAppSettings->value(outputAddressSetting()).toList();
    QList<QVariant> lAmount = mAppSettings->value(outputAmountSetting()).toList();

    lAddress.append(iAddress);
    lAmount.append(iAmount);

    mAppSettings->setValue(outputAddressSetting(), lAddress);
    mAppSettings->setValue(outputAmountSetting(), lAmount);

    mAppSettings->sync();
}

void MainWindow::savePrivateKeysToSettings(QStringList iPrivateKeys)
{
    mAppSettings->setValue(privateKeySetting(), iPrivateKeys);
    mAppSettings->sync();
}

void MainWindow::saveRawTransactionForSigningToSettings(QString iTransaction)
{
    mAppSettings->setValue(rawTransactionForSigningSetting(), iTransaction);
    mAppSettings->sync();
}

void MainWindow::startProgressBarAndDisable()
{
    ui->progressBar->setVisible(true);
    ui->tabWidget->setEnabled(false);
    ui->statusBar->showMessage("Updating", 0);
}

void MainWindow::stopProgressBarAndEnable()
{
    ui->progressBar->setVisible(false);
    ui->tabWidget->setEnabled(true);
    ui->statusBar->clearMessage();
}

void MainWindow::on_savedWalletTableWidget_cellClicked(int row, int column)
{
    Q_UNUSED(column)

    BitcoinWallet lSelectedWallet(ui->savedWalletTableWidget->item(row, 0)->data(Qt::UserRole).toByteArray());
    QString lOutput;

    lOutput.append(QString("New Wallet Address:\n  %1\n\n").arg(QString(lSelectedWallet.address())));
    lOutput.append(QString("New Wallet Wif:\n  %1\n\n").arg(QString(lSelectedWallet.wif())));
    lOutput.append(QString("New Wallet Private Key:\n  %1\n\n").arg(QString(lSelectedWallet.privateKey())));
    lOutput.append(QString("New Wallet Public Key:\n  %1\n\n").arg(QString(lSelectedWallet.publicKey())));

    ui->savedWalletDetailsLabel->setText(lOutput);
}

void MainWindow::on_deletePushButton_released()
{
    if (ui->deleteCheckBox->isChecked()) {
        int lCurrentRow = ui->savedWalletTableWidget->currentRow();
        qDebug() << "Selected Index = " << QString::number(lCurrentRow);

        ui->deleteCheckBox->setChecked(false);

        if (lCurrentRow != -1) {
            QByteArray lAddressToDelete = BitcoinWallet(ui->savedWalletTableWidget->item(lCurrentRow, 0)->data(Qt::UserRole).toByteArray()).address();
            QList<QVariant> lSavedSettingWallets;

            ui->savedWalletTableWidget->removeRow(lCurrentRow);
            ui->savedWalletDetailsLabel->clear();

            mAppSettings->remove(walletSetting());

            for (int i = 0; i < mWallets.size(); i++) {
                if (mWallets.at(i).address() == lAddressToDelete) {
                    mWallets.removeAt(i);
                } else {
                    lSavedSettingWallets.append(mWallets.at(i).toData());
                }
            }

            mAppSettings->setValue(walletSetting(), lSavedSettingWallets);
            mAppSettings->sync();
        } else {
            ui->statusBar->showMessage("No row selected", 3000);
        }

    } else {
        ui->statusBar->showMessage("Please check the delete checkbox", 3000);
    }
}

void MainWindow::on_decodeTransactionPushButton_released()
{
    qDebug() << "Decode Transaction Started";

    mAppSettings->setValue(currentRawHex(), ui->rawTransactionPlainTextEdit->document()->toPlainText());
    mAppSettings->sync();

    QString lOutput;
    btc_tx *lTx = btc_tx_new();

    if (BlockchainTool::decodeRawTransaction(QString(ui->rawTransactionPlainTextEdit->document()->toPlainText()).toUtf8(), lOutput, currentChain())) {
        ui->statusBar->showMessage("Decode successful", 2000);
        ui->rawTransactionOutputLabel->setText(lOutput);
    } else {
        ui->statusBar->showMessage("Decode failed", 2000);
        ui->rawTransactionOutputLabel->setText(lOutput);
    }

    btc_tx_free(lTx);
}

void MainWindow::on_createRawTransactionPushButton_released()
{
    //create raw transaction
    qDebug() << "Create Raw Transaction Started...";
    mAppSettings->remove(inputUtxosSetting());
    mAppSettings->remove(inputVoutSetting());
    mAppSettings->remove(outputAddressSetting());
    mAppSettings->remove(outputAmountSetting());

    QStringList lUtxos;
    QStringList lVouts;
    QStringList lAddresses;
    QStringList lAmounts;

    for (int i = 0; i < ui->inputsTableWidget->rowCount(); i++) {
        QString lUtxo = ui->inputsTableWidget->item(i, 0)->text();
        QString lVout = ui->inputsTableWidget->item(i, 1)->text();

        if (lUtxo.isEmpty() || lVout.isEmpty()) {
            ui->statusBar->showMessage("Please complete all rows", 3000);
            return;
        }

        lUtxos << lUtxo;
        lVouts << lVout;

        saveInputToSettings(lUtxo, lVout);
    }

    for (int i = 0; i < ui->outputsTableWidget_2->rowCount(); i++) {
        QString lAddress = ui->outputsTableWidget_2->item(i, 0)->text();
        QString lAmount = ui->outputsTableWidget_2->item(i, 1)->text();

        if (lAddress.isEmpty() || lAmount.isEmpty()) {
            ui->statusBar->showMessage("Please complete all rows", 3000);
            return;
        }

        lAddresses << lAddress;
        lAmounts << lAmount;

        saveOutputToSettings(lAddress, lAmount);
    }

    QByteArray lRawTransaction;
    if (BlockchainTool::createRawTransaction(lUtxos, lVouts, lAddresses, lAmounts, lRawTransaction, currentChain())) {
        ui->rawTransactionHexPlainTextEdit->document()->setPlainText(QString(lRawTransaction));
        ui->statusBar->showMessage("Raw transaction successful", 2000);
    } else {
        ui->statusBar->showMessage("Failed to create raw transaction", 3000);
        ui->rawTransactionHexPlainTextEdit->document()->setPlainText("");
    }

}

void MainWindow::on_addUtxoPushButton_released()
{
    ui->inputsTableWidget->insertRow(ui->inputsTableWidget->rowCount());
}

void MainWindow::on_removeSelectedUtxoPushButton_4_released()
{
    ui->inputsTableWidget->removeRow(ui->inputsTableWidget->currentRow());
}

void MainWindow::on_addOutputPushButton_2_released()
{
    ui->outputsTableWidget_2->insertRow(ui->outputsTableWidget_2->rowCount());
}

void MainWindow::on_removeOutputPushButton_3_released()
{
    ui->outputsTableWidget_2->removeRow(ui->outputsTableWidget_2->rowCount());
}

void MainWindow::on_signTransactionPushButton_released()
{
    mAppSettings->remove(privateKeySetting());
    mAppSettings->remove(rawTransactionForSigningSetting());

    QStringList         lPrivateKeyStrings;
    QList<QByteArray>   lPrivateKeys;
    QString             lRawTransaction = ui->rawTransactionForSigningPlainTextEdit->document()->toPlainText();
    QByteArray          lRawTransactionHex = lRawTransaction.toUtf8();
    QByteArray          lSignedRawTransaction;

    for (int i = 0; i < ui->privateKeyForSigningTableWidget->rowCount(); i++) {
        auto lPString = ui->privateKeyForSigningTableWidget->item(i,0)->text();
        lPrivateKeyStrings << lPString;
        lPrivateKeys << lPString.toUtf8();
    }

    savePrivateKeysToSettings(lPrivateKeyStrings);
    saveRawTransactionForSigningToSettings(lRawTransaction);

    ui->statusBar->showMessage("Signing Transaction...");

    if (BlockchainTool::signRawTransaction(lRawTransactionHex, lPrivateKeys, currentChain(), lSignedRawTransaction)) {
        ui->signedTransactionHexPlainTextEdit->document()->setPlainText(QString(lSignedRawTransaction));
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage("Signing Transaction Complete", 2000);
    } else {
        ui->signedTransactionHexPlainTextEdit->document()->setPlainText("Fuck... Failed to Sign Transaction");
        ui->statusBar->clearMessage();
        ui->statusBar->showMessage("Signing Transaction Failed", 2000);
    }
}

void MainWindow::on_addPrivateKeyPushButton_released()
{
    ui->privateKeyForSigningTableWidget->insertRow(ui->privateKeyForSigningTableWidget->rowCount());
}

void MainWindow::on_removePrivateKeyPushButton_2_released()
{
    ui->privateKeyForSigningTableWidget->removeRow(ui->privateKeyForSigningTableWidget->currentRow());
}

void MainWindow::openBlockExplorerAction()
{
    // Open block explorer
    QDesktopServices::openUrl(QUrl("https://testnet.blockexplorer.com/"));
}

void MainWindow::openTestFaucetManu()
{
    QDesktopServices::openUrl(QUrl("https://testnet.manu.backend.hamburg/faucet"));
}

void MainWindow::openTestFaucetTest3()
{
    QDesktopServices::openUrl(QUrl("https://testnet.coinfaucet.eu/en/"));
}

void MainWindow::on_forNodeGetUtxoPushButton_released()
{
    QString lAddress = ui->fromAddressCreateFromNodeLineEdit->text();

    QStringList lUtxos;
    QStringList lVouts;
    QStringList lAmounts;

    if (!lAddress.isEmpty()) {
        if (BlockchainTool::getUnspentTransactions(lAddress, lUtxos, lVouts, lAmounts)) {
            if (lUtxos.count() == lVouts.count() && lVouts.count() == lAmounts.count()) {
                for (int i = 0; i < lUtxos.count(); i++) {
                    ui->fromNodeUtxoTableWidget->insertRow(i);
                    ui->fromNodeUtxoTableWidget->setItem(i, 0, new QTableWidgetItem(lAmounts.at(i)));
                    ui->fromNodeUtxoTableWidget->setItem(i, 1, new QTableWidgetItem(lUtxos.at(i)));
                    ui->fromNodeUtxoTableWidget->setItem(i, 2, new QTableWidgetItem(lVouts.at(i)));
                }
            } else {
                ui->statusBar->showMessage("Error with unspent transactions", 3000);
            }
        } else {
            ui->statusBar->showMessage("Failed to get unspent transactions", 3000);
        }
    } else {
        ui->statusBar->showMessage("Enter input address", 2000);
    }
}

void MainWindow::on_addOutputNodeCreateRawPushButton_2_released()
{
    ui->outputCreateRawFromNodeTableWidget_2->insertRow(ui->outputCreateRawFromNodeTableWidget_2->rowCount());
}

void MainWindow::on_deleteOutputNodeCreateRawPushButton_3_released()
{
    ui->outputCreateRawFromNodeTableWidget_2->removeRow(ui->outputCreateRawFromNodeTableWidget_2->currentRow());
}

void MainWindow::on_createRawTransactionFromNodePushButton_released()
{
    QStringList lInputTxids;
    QStringList lInputVouts;
    QStringList lOutputAddress;
    QStringList lOutputAmounts;

    QByteArray  lOutputRawTransaction;

    for (int i = 0; i < ui->fromNodeUtxoTableWidget->rowCount(); i++) {
        if (ui->fromNodeUtxoTableWidget->item(i, 0)->isSelected()) {
            lInputTxids.append(ui->fromNodeUtxoTableWidget->item(i, 1)->text());
            lInputVouts.append(ui->fromNodeUtxoTableWidget->item(i, 2)->text());
        }
    }

    for (int i = 0; i < ui->outputCreateRawFromNodeTableWidget_2->rowCount(); i++) {
        lOutputAddress.append(ui->outputCreateRawFromNodeTableWidget_2->item(i, 0)->text());
        lOutputAmounts.append(ui->outputCreateRawFromNodeTableWidget_2->item(i, 1)->text());
    }

    if (BlockchainTool::createRawTransactionUsingBitcoinNode(lInputTxids, lInputVouts, lOutputAddress, lOutputAmounts, lOutputRawTransaction)) {
        ui->fromNodeRawTransactionOuputPlainTextEdit->document()->setPlainText(QString(lOutputRawTransaction));
        ui->statusBar->showMessage("Raw transaction created!");
    } else {
        ui->statusBar->showMessage("Failed to create raw transaction");
    }
}

void MainWindow::on_fromNodeUtxoTableWidget_itemSelectionChanged()
{
    QStringMath lTotal;
    for (int i = 0; i < ui->fromNodeUtxoTableWidget->rowCount(); i++) {
        if (ui->fromNodeUtxoTableWidget->item(i, 0)->isSelected()) {
            lTotal = lTotal + ui->fromNodeUtxoTableWidget->item(i,0)->text();
        }
    }
    ui->runningTotalForNodeCreateLabel_2->setText(QString("Total BTC Selected: %1").arg(lTotal.toString()));
}
