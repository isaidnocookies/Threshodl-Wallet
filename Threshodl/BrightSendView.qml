import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Item {
    id: sendTab

    function stopBusyIndicatorAndEnable() {
        mWaitingLayer.visible = false;
    }

    function startBusyIndicatorAndDisable() {
        mWaitingLayer.visible = true;
    }

    Connections {
        target: userAccount

        onRawTransactionCreated: {
            stopBusyIndicatorAndEnable();

            if (success) {
                messageDialog.txhex = lHex;
                messageDialog.fee = lFee;
                messageDialog.getConfirmationMessage();

                messageDialog.open()
            } else {
                alertDialog.title = "Error"
                alertDialog.text = "There was an error creating the raw transaction"

                alertDialog.open()
            }
        }

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
        }
    }

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
            source: "qrc:/images/assets/scanQrButtonIcon.png"
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
            userAccount.createRawTransaction(walletShortName, addressTextField.text, sendAmountTextField.text);
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
