import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Item {
    id: darkWalletViewPage

    property string walletShortName
    property string walletIconPath

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
        backButton.enabled = false
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        backButton.enabled = true
        mWaitingLayer.visible = false
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

        text: getCurrencySymbol("USD") + userAccount.getTotalBalance("USD")
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
        text:  userAccount.getBalance(walletShortName, true) + " " + walletShortName

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCryptoForWallet.text = userAccount.getBalance(walletShortName, true) + " " + walletShortName

                    walletConfirmationStatusLabel.text = getConfirmationText(walletShortName, "Dark")
                    if (userAccount.isWalletConfirmed(walletShortName, "Dark")) {
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
        text: getConfirmationText(walletShortName, "Dark")
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

    Button {
        id: depositButton
        text: "Deposit"
        width: parent.width * 0.4
        height: 40
        y: walletMarketValue.y + walletMarketValue.height + 25
        x: parent.width / 2 - 10 - width

        onDownChanged: {
            if (depositButton.down === true) {
                depositButtonText.color = "darkgray"
            } else {
                depositButtonText.color = "white"
            }
        }

        onClicked: {
            console.log("Start deposit into Dark...");
        }

        contentItem: Text {
            id: depositButtonText
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: depositButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            id: depositButtonBackground
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: 20
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#111"
                }
                GradientStop {
                    position: 1
                    color: "#333"
                }
            }
        }
    }

    Button {
        id: withdrawButton
        text: "Withdraw"
        width: parent.width * 0.4
        height: 40
        y: depositButton.y
        x: parent.width / 2 + 10

        onDownChanged: {
            if (withdrawButton.down === true) {
                withdrawButtonText.color = "darkgray"
            } else {
                withdrawButtonText.color = "white"
            }
        }

        onClicked: {
            console.log("Start Withdraw from Dark...")
        }

        contentItem: Text {
            id: withdrawButtonText
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: withdrawButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            id: withdrawButtonBackground
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: 20
            border.color: "darkgray"
            border.width: 1
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#333"
                }
                GradientStop {
                    position: 1
                    color: "#666"
                }
            }
        }
    }

    Rectangle {
        id: aboveLineSpacer
        color: "white"
        anchors.top: depositButton.bottom
        height: 15
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
                color: "white"

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

            TextField {
                id: emailAddressTextField
                placeholderText: "To Email Address"
                width: parent.width * 0.80
                y: addressTextField.y + addressTextField.height + 20
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

                y: emailAddressTextField.y + emailAddressTextField.height + 15
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


            Button {
                id: sendButton
                text: "Send"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                height: 40
                y: parent.height - bottomAreaCorrectionHeight - height

                onClicked: {
                    //do some stuffs
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
                            color: "#000"
                        }
                        GradientStop {
                            position: 1
                            color: "#555"
                        }
                    }
                }
            }
        }
        Item {
            id: receiveTab

            Image{
                id: qrCodeForRecieve
                source: {
                    var email = userAccount.getEmailAddress();
                    if (email === "") {
                        "image://QZXing/encode/" + ""
                    } else {
                        "image://QZXing/encode/" + "{\"address\":\"" + userAccount.username + "\":\"email\":\"" + email + "\"}";
                    }

                }
                cache: false;
                width: parent.width * 0.5
                height: width

                anchors.horizontalCenter: parent.horizontalCenter
                y: 20
            }

            Text {
                id: qrAddressLabel
                text: "Address: " + userAccount.username
                wrapMode: Text.WrapAnywhere
                font.pointSize: 13
                font.weight: Font.Thin

                y: qrCodeForRecieve.y + qrCodeForRecieve.height + 10
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: qrEmailLabel
                text: userAccount.getEmailAddress();
                font.pointSize: 13
                font.weight: Font.Thin

                y: qrAddressLabel.y + qrAddressLabel.height + 10
                anchors.horizontalCenter: parent.horizontalCenter
            }

            TextField {
                id: setEmailAddressField
                placeholderText: "Your Email Address"
                width: parent.width * 0.80
                y: qrEmailLabel.y + qrEmailLabel.height
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                horizontalAlignment: TextInput.AlignHCenter

                visible: {
                    if (userAccount.getEmailAddress() === "") {
                        true
                    } else {
                        false
                    }
                }

                background: Rectangle {
                    radius: 20
                    border.color: "lightgray"
                    border.width: 0.5
                }
            }

            Button {
                id: addEmailAddressButton
                text: "Set Email Address"
                y: setEmailAddressField.y + setEmailAddressField.height + 10
                width: setEmailAddressField.width
                anchors.horizontalCenter: parent.horizontalCenter
                height: 35

                visible: setEmailAddressField.visible

                contentItem: Text {
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: addEmailAddressButton.text
                    font.bold: false
                    font.pointSize: 12
                }

                background: Rectangle {
                    border.color: "black"
                    border.width: 1
                    radius: height/2
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#000"
                        }
                        GradientStop {
                            position: 1
                            color: "#555"
                        }
                    }
                }

                onClicked: {
                    userAccount.setEmailAddress(setEmailAddressField.text);
                    qrCodeForRecieve.source = "image://QZXing/encode/" + "{\"address\":\"" + userAccount.username + "\":\"email\":\"" + setEmailAddressField.text + "\"}";
                    setEmailAddressField.visible = false;
                    addEmailAddressButton.visible = false;
                    qrEmailLabel.text = setEmailAddressField.text;
                }
            }
        }

        Item {
            id: transactionsTab
        }
    }
}
