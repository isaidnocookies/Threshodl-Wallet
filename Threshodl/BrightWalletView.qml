import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

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
        onRawTransactionSent: {
            stopBusyIndicatorAndEnable();

            if (success) {
                console.log("Success!")
                sendTimeoutTimer.stop();
                alertDialog.title = "Transaction Successful";
                alertDialog.text = "The transaction has been completed. Please wait for the transaction to be confirmed\n\n" + lTxid;
                alertDialog.open();
            } else {
                console.log("Failed to send...")
                alertDialog.title = "Transaction Failed"
                alertDialog.text = "The transaction has failed to send. Please try again"
                alertDialog.open()
            }
        }

        onRawTransactionCreated: {
            stopBusyIndicatorAndEnable();

            if (success) {
                messageDialog.txhex = lHex;
                messageDialog.fee = lFee;

                messageDialog.open()
            } else {
                alertDialog.title = "Error"
                alertDialog.text = "There was an error creating the raw transaction"

                alertDialog.open()
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
                marketValueText.text = "(Market Value: $" + threshodlTools.formatMarketValueString(getMarketValue(shortName)) + ")"
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

        Item {
            id: sendTab

            TextField {
                id: sendAmountTextField
                placeholderText: "Amount to Send"
                width: parent.width * 0.80
                y: 20
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                horizontalAlignment: TextInput.AlignHCenter

                background: Rectangle {
                    radius: 20
                    border.color: "lightgray"
                    border.width: 0.5
                }

                onFocusChanged: {
                }
            }

            TextField {
                id: addressTextField
                placeholderText: "To Address"
                width: parent.width * 0.80
                y: sendAmountTextField.y + sendAmountTextField.height + 20
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                horizontalAlignment: TextInput.AlignHCenter

                background: Rectangle {
                    radius: 20
                    border.color: "lightgray"
                    border.width: 0.5
                }

                onFocusChanged: {
                }
            }

            Button {
                id: scanQrCodeButton
                height: 30
                width: 30

                y: addressTextField.y + addressTextField.height + 15
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: "white"
                    width: parent.height
                    height: parent.width
                    anchors.centerIn: parent
                }

                Image {
                    source: "images/assets/scanQrButtonIcon.png"
                    fillMode: Image.PreserveAspectFit
                    width: parent.width
                }

                onClicked: {
                    myQrScanner.restartTimer()
                    ourStackView.push(myQrScanner)
                }
            }

            Text {
                id: warningLabel
                y: sendButton.y - height - 20
                anchors.horizontalCenter: parent.horizontalCenter
                color: "red"
                text: ""

                onTextChanged: {
                    console.log("Warning label text changed")
                    warningLabelTimer.start()
                }
            }

            Timer {
                id: warningLabelTimer
                repeat: false
                running: false
                interval: 5000

                onTriggered: {
                    warningLabel.text = ""
                    console.log("Warning text reset")
                }
            }

            MessageDialog {
                id: alertDialog
                title: ""
                text: ""

                onAccepted: {
                    alertDialog.visible = false
                }
            }

            MessageDialog {
                id: messageDialog

                property string txhex;
                property string fee;

                function getConfirmationMessage() {
                    return "\n\n Amount: " + sendAmountTextField.text + " " + walletShortName + " \n\nTo: \n" + addressTextField.text + "\n\nFee:\n" + fee + " " + walletShortName + "\n\nConfirm Transaction?"
                }

                title: "Confirm Transaction"
                text: ""

                standardButtons: StandardButton.Cancel | StandardButton.Yes

                onTxhexChanged: {
                    messageDialog.text = getConfirmationMessage()
                }

                onFeeChanged: {
                    messageDialog.text = getConfirmationMessage()
                }

                onYes: {
                    console.log("Transaction Confirmed.")
                    messageDialog.visible = false

                    startBusyIndicatorAndDisable();
                    userAccount.sendRawTransaction(walletShortName, txhex);
                }

                onRejected: {
                    console.log("Cancel")
                    warningLabel.text = "Transaction was canceled!"
                }
            }

            MessageDialog {
                id: lowFeeDialog

                title: "Transaction Fee"
                text: "Sending this amount of " + walletShortName + " will result in some being used for the network fee."

                standardButtons: StandardButton.Cancel | StandardButton.Yes

                onYes: {
                    console.log("Transaction with low fees confirmed.")
                    messageDialog.visible = false

                    startBusyIndicatorAndDisable();
                    userAccount.getRawTransaction(walletShortName, addressTextField.text, sendAmountTextField.text);
                }

                onRejected: {
                    console.log("Cancel")
                    warningLabel.text = "Transaction was canceled!"
                }
            }

            Timer {
                id: sendTimeoutTimer
                repeat: false
                running: false
                interval: 20000

                onTriggered: {
                    console.log("Send timeout occured")
                    stopBusyIndicatorAndEnable();
                    warningLabel.text = "Transaction failed!"
                }
            }

            Button {
                id: sendButton
                text: "Send"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                height: 40
                y: parent.height - bottomAreaCorrectionHeight - height

                onClicked: {
                    if (addressTextField.text === "" || sendAmountTextField.text === "") {
                        warningLabel.text = "Please complete the fields above"
                    } else if (parseFloat(userAccount.getBalance(walletShortName, false)) < parseFloat(sendAmountTextField.text)) {
                        warningLabel.text = "Not enough coin in wallet!"
                    } else if (parseFloat(userAccount.getBalance(walletShortName, false)) === parseFloat(sendAmountTextField.text)) {
                        lowFeeDialog.open()
                    } else {
                        startBusyIndicatorAndDisable();
                        sendTimeoutTimer.start()
                        userAccount.createRawTransaction(walletShortName, addressTextField.text, sendAmountTextField.text);
                    }
                }

                contentItem: Text {
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: sendButton.text
                    font.bold: true
                    font.pointSize: buttonFontSize
                }

                background: Rectangle {
                    width: parent.height
                    height: parent.width
                    anchors.centerIn: parent
                    rotation: 270
                    radius: 20
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#6796FF"
                        }
                        GradientStop {
                            position: 1
                            color: "#00CFF5"
                        }
                    }
                }
            }
        }

        Item {
            id: receiveTab

            MessageDialog {
                id: walletCreationAlert
                title: ""
                text: ""

                onAccepted: {
                    alertDialog.visible = false
                }
            }

            Image{
                id: qrCodeForRecieve
                source: ""
                cache: false;
                width: parent.width * 0.5
                height: width

                anchors.horizontalCenter: parent.horizontalCenter
                y: 20

                Component.onCompleted: {
                    if (userAccount.getBrightAddress(walletShortName) === "") {
                        qrCodeForRecieve.source = "images/assets/addWalletIcon.png"
                    } else {
                        qrCodeForRecieve.source = "image://QZXing/encode/" + userAccount.getBrightAddress(walletShortName);
                    }
                }

                MouseArea {
                    //click to add wallet...
                    anchors.fill: parent
                    onClicked: {
                        console.log("Try to create wallet....")
                        startBusyIndicatorAndDisable()
                        var brightAddress = userAccount.createBrightWallet(walletShortName)
                        if (brightAddress === "") {
                            walletCreationAlert.title = "Wallet Creation Failed!"
                            walletCreationAlert.text = walletShortName + " wallet creation failed! Please try again or contact support."
                            walletCreationAlert.open()
                        } else {
                            qrCodeForRecieve.source = "image://QZXing/encode/" + userAccount.getBrightAddress(walletShortName)
                            qrAddressLabel.text = brightAddress
                            copyQrAddressButton.visible = true
                            walletCreationAlert.title = "Wallet Created!"
                            walletCreationAlert.text = walletShortName + " wallet was created successfully! Your wallet is ready to accept transactions."
                            walletCreationAlert.open()
                        }

                        stopBusyIndicatorAndEnable()
                    }
                }
            }

            TextInput {
                id: qrAddressLabel
                text: getBrightAddress()
                wrapMode: Text.WrapAnywhere
                font.pointSize: 13
                font.weight: Font.Thin
                color: "black"

                y: qrCodeForRecieve.y + qrCodeForRecieve.height + 10
                anchors.horizontalCenter: parent.horizontalCenter

                readOnly: true
                selectByMouse: true

                function getBrightAddress() {
                    var brightAddress = userAccount.getBrightAddress(walletShortName);
                    if (brightAddress === "") {
                        return "Click to create new wallet"
                    } else {
                        return brightAddress
                    }
                }
            }

            Button {
                id: copyQrAddressButton
                height: 20
                width: 20

                visible: (userAccount.getBrightAddress(walletShortName) !== "")

                y: qrAddressLabel.y + qrAddressLabel.height + 10
                anchors.horizontalCenter: parent.horizontalCenter

                background: Rectangle {
                    color: "white"
                    width: parent.height
                    height: parent.width
                    anchors.centerIn: parent
                }

                Image {
                    source: "images/assets/clipboardPasteIcon.png"
                    fillMode: Image.PreserveAspectFit
                    width: parent.width
                }

                onPressed: {
                    height = height - 2;
                    width = width - 2;
                }
                onReleased: {
                    height = height + 2;
                    width = width + 2;
                }

                onClicked: {
                    threshodlTools.copyToClipboard(qrAddressLabel.text)
                    warningLabelReceive.text = "Address Copied!"
                }
            }

            Text {
                id: warningLabelReceive
                y: parent.height - 60
                anchors.horizontalCenter: parent.horizontalCenter
                color: "red"
                text: ""

                onTextChanged: {
                    console.log("Warning label text changed")
                    warningLabelTimerReceive.start()
                }
            }

            Timer {
                id: warningLabelTimerReceive
                repeat: false
                running: false
                interval: 5000

                onTriggered: {
                    warningLabelReceive.text = ""
                    console.log("Warning text reset")
                }
            }
        }

        Item {
            id: transactionsTab
        }
    }
}
