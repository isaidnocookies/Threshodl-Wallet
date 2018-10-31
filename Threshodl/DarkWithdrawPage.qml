import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/darkWalletView/";

Item {
    id: darkWalletWithdrawPage

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

        // darkWithdrawalEstimated(bool success, QString fee);
        onDarkWithdrawalEstimated : {
            withdrawConfirmation.setForEstimate(success, fee);
            withdrawConfirmation.open();
        }

        // darkWithdrawComplete(bool success, int error);
        onDarkWithdrawComplete : {
            withdrawConfirmation.setForCompletion(success);
            withdrawConfirmation.open();
        }
    }

    MessageDialog {
        id: withdrawConfirmation

        property bool forEstimate

        function setForEstimate(success, fee){
            withdrawConfirmation.title = "Dark Withdraw Confirmation"
            withdrawConfirmation.standardButtons = StandardButton.No | StandardButton.Yes
            forEstimate = true;
            if (success) {
                withdrawConfirmation.text = "Are you sure you would like to withdraw " + walletShortName + " from your dark wallet and into your normal 'Bright' wallet? \n\n Fee: " + fee + " " + walletShortName;
            } else {
                withdrawConfirmation.text = "Fee estimation failed. Please try again";
            }
        }

        function setForCompletion(success) {
            withdrawConfirmation.title = "Dark Withdraw"
            withdrawConfirmation.standardButtons = StandardButton.Ok
            forEstimate = false;
            if (success) {
                withdrawConfirmation.text = "Dark withdraw has been completed! Your balance should be reflected in your normal 'bright' wallet shortly"
            }
        }

        title: ""
        text: ""

        onAccepted: {
            if (forEstimate) {
                //start withdrawal process
                userAccount.withdrawDarkCoin(walletShortName, amountTextField.text);
            } else {
                console.log("Transaction confirmation accepted")
                withdrawConfirmation.close();
                stopBusyIndicatorAndEnable();
            }
        }

        onRejected: {
            stopBusyIndicatorAndEnable();
            warningLabel.text = "Withdrawal Canceled!"
            console.log("No withdraw for you.")
        }

        onNo: {
            stopBusyIndicatorAndEnable();
            warningLabel.text = "Withdrawal Canceled!"
            console.log("No withdraw for you.")
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
                color: "#12003f"
            }
            GradientStop {
                position: 0.3
                color: "#500088"
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

    Image {
        id: topBarIcon
        source: Qt.resolvedUrl(walletIconPath)

        width: 60
        height: 60

        y: backButton.y
        x: parent.width/2 - width/2
    }

    Text {
        id: title
        y: topBarIcon.y + (topBarIcon.height) + 40
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Withdraw From " + walletShortName;
        font.pointSize: 20

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
            text = "You are withdrawing from your Dark account. This will transfer coin out of your dark wallet and into your normal " + getBaseShortname(walletShortName) + " wallet associated with this coin. This will incur an \"exit\" fee as it transfers funds from your dark wallets to your normal \"bright\" wallets."
        }
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 13
    }

    TextField {
        id: amountTextField
        placeholderText: "Amount to Withdraw"
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
        text:  ""

        Component.onCompleted: {
            text = "(Available Balance: " + userAccount.getBalance(walletShortName, true) + " " + walletShortName + ")"
        }

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname === walletShortName) {
                    totalCryptoForWallet.text = "(Available Balance: " + userAccount.getBalance(walletShortName, true) + " " + walletShortName + ")"
                }
            }
        }
    }

    Text {
        id: warningLabel
        y: withdrawButton.y - 20 - height
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
        id: withdrawButton
        text: "Withdraw"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: 40
        y: parent.height - bottomAreaCorrectionHeight - height

        onDownChanged: {
            if (depositButton.down === true) {
                withdrawButtonText.color = "darkgray"
            } else {
                withdrawButtonText.color = "white"
            }
        }

        onClicked: {
            console.log("Start withdraw from Dark...");
            startBusyIndicatorAndDisable();
            userAccount.estimateDarkWithdrawal(walletShortName, amountTextField.text);
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
            gradient: Gradient {
                GradientStop {
                    position: 1
                    color: "#12003f"
                }
                GradientStop {
                    position: 0
                    color: "#500088"
                }
            }
        }
    }
}
