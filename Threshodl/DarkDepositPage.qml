import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/darkWalletView/";

Item {
    id: darkWalletDepositPage

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

    function getBaseShortname(iShortname) {
        var editedShortname = iShortname;

        if (editedShortname.charAt(0) === "d") {
            editedShortname = editedShortname.substring(1);
        }

        return editedShortname
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

    Connections {
        target: userAccount

        onDarkDepositConfirmation : {
            stopBusyIndicatorAndEnable();
            if (oSuccess) {
                console.log("Success on first thing for deposit thing");
                console.log("Fee:  " + oFee + "   WithoutFee: " + oActualAmountWithoutFee + "   Shortname: " + oShortname)
                messageDialog.fee = oFee;
                messageDialog.amountWithoutFee = oActualAmountWithoutFee;
                messageDialog.text = messageDialog.getConfirmationMessage();
                messageDialog.standardButtons = StandardButton.Cancel | StandardButton.Yes;
                messageDialog.open()
            } else {
                console.log("Stupid fucking deposit thing brokeded");
            }
        }

        onDarkDepositComplete : {
            stopBusyIndicatorAndEnable();

            if (oSuccess) {
                console.log("Dark Deposit Complete!");
                console.log("Breaks:  " + oBreaks + "   WithoutFee: " + oActualAmountWithoutFees)
                messageDialog.text = messageDialog.getCompletedMessage();
                messageDialog.standardButtons = StandardButton.Ok;
                messageDialog.open();
            } else {
                // trigger a confirmation of FUCKING FAILURE
                console.log("Stupid fucking deposit confirmation");
            }
        }

        onWalletBalanceUpdateComplete: {
            if (shortname === getBaseShortname(walletShortName)) {
                totalCryptoForWallet.text = "(Available Balance: " + userAccount.getBalance(getBaseShortname(walletShortName), true) + " " + getBaseShortname(walletShortName) + ")"
            }
        }
    }

    MessageDialog {
        id: messageDialog

        property string amountWithoutFee;
        property string fee;

        property bool completed: false;

        function getConfirmationMessage() {
            completed = false;
            console.log("The wallet shortname is... " + walletShortName);
            return "\n\n Amount: " + amountTextField.text + " " + getBaseShortname(walletShortName) + "\n\nFee:\n" + fee + " " + getBaseShortname(walletShortName) + "\n\nTotal Amount to turn dark:\n" + amountWithoutFee + " " + getBaseShortname(walletShortName) + "\n\nConfirm Transaction?"
        }

        function getCompletedMessage() {
            completed = true;
            return "Deposit Complete!"
        }

        title: "Dark Deposit"
        text: ""

        standardButtons: StandardButton.Cancel | StandardButton.Yes

        onYes: {
            console.log("Deposit Approved.")
            messageDialog.visible = false;

            if (!completed) {
                startBusyIndicatorAndDisable();
                userAccount.depositDarkCoin(walletShortName, amountTextField.text, fee);
            } else {
                warningLabel.text = "Deposit Completed";
            }

        }

        onRejected: {
            console.log("Cancel")
            warningLabel.text = "Dark deposit was canceled!"
        }

        onAccepted: {
            console.log("Confirmation dismissed");
            amountTextField.text = "";
            ourStackView.pop();
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

    Rectangle {
        id: topBackgroundShape
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#00223f"
            }
            GradientStop {
                position: 0.3
                color: "#064880"
            }
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

    Text {
        id: title
        y: topBarIcon.y + (topBarIcon.height) + 40
        anchors.horizontalCenter: parent.horizontalCenter
        text: ""//"Deposit From " + getBaseShortname(walletShortName);
        font.pointSize: 20

        Component.onCompleted: {
            text = "Deposit From " + getBaseShortname(walletShortName);
        }

        color: "white"
        font.bold: true
    }

    Rectangle {
        id: bottomBackgroundShape
        radius: 25
        width: parent.width
        anchors.top: title.bottom
        anchors.topMargin: 30
        anchors.bottom: parent.bottom
    }

    Text {
        id: descriptionLabel
        y: bottomBackgroundShape.y + 20
        width: parent.width * 0.80
        text: ""
        Component.onCompleted: {
            text = "You are depositing into your Dark account. This will transfer coin out of your " + getBaseShortname(walletShortName) + " wallet and into your Dark wallet associated with this coin. This wallet will be untraceable during transactions and completely off-the-blockchain. This will incur a fee as it breaks down your funds into smaller micro-wallets."
        }
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 13
    }

    TextField {
        id: amountTextField
        placeholderText: "Amount to Deposit"
        width: parent.width * 0.80
        y: descriptionLabel.y + descriptionLabel.height + 25
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

    Text {
        id: totalCryptoForWallet
        z:10

        font.bold: true
        font.pointSize: 16

        y: amountTextField.y + amountTextField.height + 20
        anchors.horizontalCenter: parent.horizontalCenter
        text: ""
        Component.onCompleted: {
            text = "(Available Balance: " + userAccount.getBalance(getBaseShortname(walletShortName), true) + " " + getBaseShortname(walletShortName) + ")"
        }
    }

    Text {
        id: warningLabel
        y: depositButton.y - 20 - height
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

    Button {
        id: depositButton
        text: "Deposit"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: 40
        y: parent.height - bottomAreaCorrectionHeight - height

        onDownChanged: {
            if (depositButton.down === true) {
                depositButtonText.color = "darkgray"
            } else {
                depositButtonText.color = "white"
            }
        }

        onClicked: {
            // Sanity check on value in field
            if (amountTextField.text === "") {
                warningLabel.text = "Please input amount!"
            } /*else if (parseFloat(amountTextField.text) > parseFloat(userAccount.getBalance(getBaseShortname(walletShortName), true))) {
                warningLabel.text = "Please input smaller amount"
            }*/ else {
                console.log("Start deposit into Dark...");
                startBusyIndicatorAndDisable();
                userAccount.startDarkDeposit(walletShortName, amountTextField.text)
            }

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
                    color: "#00223f"
                }
                GradientStop {
                    position: 1
                    color: "#064880"
                }
            }
        }
    }
}
