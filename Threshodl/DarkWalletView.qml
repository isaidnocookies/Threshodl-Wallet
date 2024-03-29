import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/darkWalletView/";

Item {
    id: darkWalletViewPage

    property string walletShortName
    property string walletIconPath

    Rectangle {
        anchors.fill: parent
        color: "white";
    }

    Component.onCompleted: {
        darkDepositPageView.walletShortName = walletShortName
        darkWithdrawPageView.walletShortName = walletShortName

        darkDepositPageView.walletIconPath = walletIconPath
        darkWithdrawPageView.walletIconPath = walletIconPath
    }

    function getConfirmationText (iShortname, walletType) {
        if (userAccount.isWalletConfirmed(iShortname, walletType) === true) {
            return "(Confirmed)";
        } else {
            var lValue = userAccount.getBalance(iShortname, false);
            return "(" + lValue + " " + iShortname + " Balance Confirming)";
        }
    }

    function getMarketValue(iShortname) {
        var strippedShortname = iShortname;

        if (strippedShortname.charAt(0) === "d") {
            strippedShortname = strippedShortname.substring(1)
        }

        if (strippedShortname.charAt(0) === "t") {
            strippedShortname = strippedShortname.substring(1)
        }

        return userAccount.getMarketValue(strippedShortname)
    }

    function getBalanceValue(iShortname) {
        return userAccount.getBalanceValue(iShortname)
    }

    function startBusyIndicatorAndDisable() {
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        mWaitingLayer.visible = false
    }

    Rectangle {
        z:0
        anchors.fill: parent
        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#000000"
            }
            GradientStop {
                position: 0.5
                color: "#222222"
            }
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

        MouseArea {
            anchors.fill: parent
            anchors.centerIn: parent
            onClicked: {
                //do nothing...
            }
        }
    }

    QrScannerView {
        id: myQrScanner
        isForDark: true

        onAddressFromScannerChanged: {
            addressTextField.text = addressFromScanner
        }

        onEmailFromScannerChanged: {
            emailAddressTextField.text = emailFromScanner
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
            source: "qrc:/images/assets/whiteBackButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: ourStackView.pop()
    }

    ViewDarkMicroWalletsPage {
        id: microWalletVewPage
        visible: false
        shortName: walletShortName
    }

    Button {
        id: walletButton
        height: 35
        width: 35

        background: Rectangle {
            color: "transparent"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: parent.width - width - 25

        Image {
            source: "qrc:/images/assets/walletIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: {
            ourStackView.push(microWalletVewPage)
            microWalletVewPage.populateTable();
        }
    }

    Text {
        id: totalCurrencyForWallet
        z:10

        font.pointSize: 50
        wrapMode: Text.NoWrap

        color: "white"

        font.weight: Font.ExtraLight

        text: totalCurrencyForWallet.text = getCurrencySymbol("USD") + userAccount.getBalanceValue(walletShortName)
        y: topBarIcon.y + topBarIcon.height + 20
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: userAccount

            onMarketValueChanged: {
                if (shortname === walletShortName || shortname === getBaseShortname(walletShortName)) {
                    totalCurrencyForWallet.text = getCurrencySymbol("USD") + userAccount.getBalanceValue(walletShortName)
                }
            }

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCurrencyForWallet.text = getCurrencySymbol("USD") + userAccount.getBalanceValue(walletShortName)
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
        text:  userAccount.getBalance(walletShortName, false) + " " + walletShortName

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCryptoForWallet.text = userAccount.getBalance(walletShortName, false) + " " + walletShortName

                    walletConfirmationStatusLabel.text = getConfirmationText(walletShortName, "Dark")
                    if (userAccount.isWalletConfirmed(walletShortName, "Dark")) {
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
        text: getConfirmationText(walletShortName, "Dark")

        color: {
            if (userAccount.isWalletConfirmed(walletShortName, "Dark")) {
                "#c1ffba"
            } else {
                "#ff9756"
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

    DarkDepositPage {
        id: darkDepositPageView

        walletShortName: walletShortName
        walletIconPath: walletIconPath

        visible: false
    }

    DarkWithdrawPage {
        id: darkWithdrawPageView

        walletShortName: walletShortName
        walletIconPath: walletIconPath

        visible: false
    }

    Button {
        id: depositButton
        width: 45
        height: 45
        y: walletMarketValue.y + walletMarketValue.height + 20
        x: parent.width / 2 - 10 - width

        onClicked: {
            console.log("Start deposit into Dark...");
            ourStackView.push(darkDepositPageView)
        }

        Image {
            source: "qrc:/images/assets/darkDepositIcon.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            height: parent.height * 0.6
        }

        background: Rectangle {
            id: depositButtonBackground
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: parent.height/2

            border.color: "#404040"
            color: "black"

//            gradient: Gradient {
//                GradientStop {
//                    position: 0
//                    color: "#00223f"
//                }
//                GradientStop {
//                    position: 1
//                    color: "#064880"
//                }
//            }
        }
    }

    Button {
        id: withdrawButton
        width: depositButton.width
        height: depositButton.height
        y: depositButton.y
        x: parent.width / 2 + 10

        onClicked: {
            console.log("Start withdraw from Dark...");
            ourStackView.push(darkWithdrawPageView)
        }

        Image {
            source: "qrc:/images/assets/darkWithdrawIcon.png"
            anchors.centerIn: parent
            fillMode: Image.PreserveAspectFit
            height: parent.height * 0.6
        }

        background: Rectangle {
            id: withdrawButtonBackground
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: parent.height/2

            color: "black"
            border.color: "#404040"
        }
    }

    Rectangle {
        id: aboveLineSpacer
        color: "white"
        anchors.top: depositButton.bottom
        height: 35
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    Rectangle {
        id: bottomSectionBackground
        width: parent.width
        anchors.top: depositButton.bottom
        anchors.topMargin: 20
        anchors.bottom: parent.bottom
        color:"white"
        radius: 25
        z:0
    }

    TabBar {
        id: miniNavBar
        width: parent.width
        height: 70

        anchors.top: aboveLineSpacer.bottom

        background: Rectangle {
            anchors.fill: parent
            color: "transparent"
        }

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
                    color: "#000000"
                    height: 2
                    anchors.bottom: parent.bottom
                    width: parent.width * 0.8
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        TabButton {
            id: receiveTabButton
            text: "Recieve"

            contentItem: Text {
                color: "black"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: receiveTabButton.text
                font.bold: true
                font.pointSize: 12
            }

            background: Rectangle {
                anchors.fill: parent
                color: "transparent"

                Rectangle {
                    visible: miniNavBar.currentIndex == 1 ? true : false
                    color: "#000000"
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

        DarkSendView {
            id: sendTab
        }

        DarkReceiveView {
            id: recieveTab
        }

        Item {
            id: transactionsTab
        }
    }
}
