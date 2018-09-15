import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/brightWalletView/";

Item {
    id: brightWalletViewPage

    property string walletShortName
    property string walletIconPath

    property string transactionHex

    function getConfirmationText (iShortname, walletType) {
        if (userAccount.isWalletConfirmed(iShortname, walletType) === true) {
            return "(Confirmed)"
        } else {
            var lValue
            if (walletType === "Dark") {
                lValue = userAccount.getBalance(iShortname, false)
            } else {
                lValue = userAccount.getBalance(iShortname, false)
            }
            return "(" + lValue + " " + iShortname + " Balance Confirming)"
        }
    }

    function getMarketValue(iShortname) {
        return userAccount.getMarketValue(iShortname)
    }

    function getBalanceValue(iShortname) {
        return userAccount.getBalanceValue(iShortname)
    }

    function startBusyIndicatorAndDisable() {
        backButton.enabled = false
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        backButton.enabled = true
        mWaitingLayer.visible = false
    }

    Connections {
        target: userAccount
//        onRawTransactionSent: {
//            stopBusyIndicatorAndEnable();

//            if (success) {
//                console.log("Success!")
//                sendTimeoutTimer.stop();
//                sendTab.alertDialog.title = "Transaction Successful";
//                sendTab.alertDialog.text = "The transaction has been completed. Please wait for the transaction to be confirmed\n\n" + lTxid;
//                sendTab.alertDialog.open();
//            } else {
//                console.log("Failed to send...")
//                sendTab.alertDialog.title = "Transaction Failed"
//                sendTab.alertDialog.text = "The transaction has failed to send. Please try again"
//                sendTab.alertDialog.open()
//            }
//        }

//        onRawTransactionCreated: {
//            stopBusyIndicatorAndEnable();

//            if (success) {
//                sendTab.messageDialog.txhex = lHex;
//                sendTab.messageDialog.fee = lFee;

//                sendTab.messageDialog.open()
//            } else {
//                sendTab.alertDialog.title = "Error"
//                sendTab.alertDialog.text = "There was an error creating the raw transaction"

//                sendTab.alertDialog.open()
//            }
//        }
    }

    Rectangle {
        anchors.fill: parent
        color: "white";
    }

    QrScannerView {
        id: myQrScanner

        onAddressFromScannerChanged: {
            console.log("Bright wallet to address changed")
            addressTextField.text = addressFromScanner
        }
    }

    Item {
        id: mWaitingLayer

        visible: false
        anchors.fill: parent
        z:100

        Rectangle {
            anchors.fill: parent
            anchors.centerIn: parent
            color: Qt.rgba(0,0,0,0.5)
        }

        BusyIndicator {
            z:101
            running: mWaitingLayer.visible
            anchors.centerIn: parent
        }
    }

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
    }

    Image {
        id: topBarIcon
        source: Qt.resolvedUrl(walletIconPath)

        width: 60
        height: 60

        y: backButton.y
        x: parent.width/2 - width/2
    }

    Button {
        id: backButton
        height: 30
        width: 30

        background: Rectangle {
            color: "white"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: 25

        Image {
            source: "images/assets/backButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: ourStackView.pop()
    }

    Text {
        id: totalCurrencyForWallet
        z:10

        font.pointSize: 60
        wrapMode: Text.NoWrap

        font.weight: Font.ExtraLight

        text: getCurrencySymbol("USD") + userAccount.getBalanceValue(walletShortName)
        y: topBarIcon.y + topBarIcon.height + 20
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCurrencyLabel.text = getCurrencySymbol("USD") + userAccount.getBalanceValue(walletShortName)
                }
            }
        }
    }

    Text {
        id: totalCryptoForWallet
        z:10

        font.bold: true
        font.pointSize: 20

        anchors.top: totalCurrencyForWallet.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: userAccount.getBalance(walletShortName, true) + " " + walletShortName

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCryptoForWallet.text = userAccount.getBalance(walletShortName, true) + " " + walletShortName

                    walletConfirmationStatusLabel.text = getConfirmationText(walletShortName, "Bright")
                    if (userAccount.isWalletConfirmed(walletShortName, "Bright")) {
                        walletConfirmationStatusLabel.color = "#116F00"
                    } else {
                        walletConfirmationStatusLabel.color = "red"
                    }
                }
            }
        }
    }

    Text {
        id: walletConfirmationStatusLabel
        z:10

        font.pointSize: 13
        y: totalCryptoForWallet.y + totalCryptoForWallet.height + 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: getConfirmationText(walletShortName, "Bright")

        color: {
            if (userAccount.isWalletConfirmed(walletShortName, "Bright")) {
                walletConfirmationStatusLabel.color = "#116F00"
            } else {
                walletConfirmationStatusLabel.color = "red"
            }
        }
    }

    Text {
        id: walletMarketValue
        z:10

        font.pointSize: 13

        y: walletConfirmationStatusLabel.y + walletConfirmationStatusLabel.height + 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: {
            if (threshodlTools.formatMarketValueString(getMarketValue(shortName)) === "0.00") {
                ""
            } else {
                "(Market Value: $" + threshodlTools.formatMarketValueString(getMarketValue(shortName)) + ")"
            }
        }

        Connections {
            target: userAccount
            onMarketValueChanged: {
                walletMarketValue.text = "(Market Value: $" + threshodlTools.formatMarketValueString(getMarketValue(shortName)) + ")"
            }
        }
    }

    Rectangle {
        id: aboveLineSpacer
        color: "white"
        anchors.top: walletMarketValue.bottom
        height: 30
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    TabBar {
        id: miniNavBar
        width: parent.width
        height: 70

        anchors.top: aboveLineSpacer.bottom

        ItemDelegate {
            id: tabButtonDelegate
            font.bold: true
        }

        TabButton {
            id: sendTabButton
            text: "Send"

            contentItem: Text {
                color: "black"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: sendTabButton.text
                font.bold: true
                font.pointSize: 13
            }

            background: Rectangle {
                anchors.fill: parent
                Rectangle {
                    visible: miniNavBar.currentIndex == 0 ? true : false
                    color: "#31B4FA"
                    height: 2
                    anchors.bottom: parent.bottom
                    width: parent.width * 0.8
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        TabButton {
            id: recieveTabButton
            text: "Receive"

            contentItem: Text {
                color: "black"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: recieveTabButton.text
                font.bold: true
                font.pointSize: 12
            }

            background: Rectangle {
                color: "white"

                Rectangle {
                    visible: miniNavBar.currentIndex == 1 ? true : false
                    color: "#31B4FA"
                    height: 2
                    anchors.bottom: parent.bottom
                    width: parent.width * 0.8
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    StackLayout {
        width: parent.width
        anchors.top: miniNavBar.bottom
        anchors.bottom: bottomBarCorrectionSpacer.bottom

        currentIndex: miniNavBar.currentIndex

        BrightSendView {
            id: sendTab
        }

        BrightReceiveView {
            id: receiveTab
        }

        Item {
            id: transactionsTab
        }
    }
}
