import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/brightWalletView/"

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

    Rectangle {
        z:0
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#0053b2"
            }
            GradientStop {
                position: 0.5
                color: "#00CFF5"
            }
        }
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
        color: "transparent"
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
            color: "transparent"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: 25

        Image {
            source: "images/assets/whiteBackButtonIcon.png"
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

        color: "white"

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

        color: "white"

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
                        walletConfirmationStatusLabel.color = "#c1ffba"
                    } else {
                        walletConfirmationStatusLabel.color = "#ff9756"
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
                walletConfirmationStatusLabel.color = "#c1ffba"
            } else {
                walletConfirmationStatusLabel.color = "#ff9756"
            }
        }
    }

    Text {
        id: walletMarketValue
        z:10

        font.pointSize: 13

        color: "white"

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
        color: "transparent"
        anchors.top: walletMarketValue.bottom
        height: 30
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "transparent"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    Rectangle {
        id: bottomSectionBackground
        width: parent.width
        anchors.top: walletMarketValue.bottom
        anchors.topMargin: 15
        anchors.bottom: parent.bottom
        color:"white"
        radius: 25
        z:0
    }

    TabBar {
        id: miniNavBar
        width: parent.width
        height: 70

        z: 100

        background: Rectangle {
            anchors.fill: parent
            color: "transparent"
        }

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
                color: "transparent"
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
                color: "transparent"

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
