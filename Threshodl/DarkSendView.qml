import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Item {
    id: sendTab

    property bool fieldsComplete: false

    TextField {
        id: sendAmountTextField
        placeholderText: "Amount to Send"
        width: parent.width * 0.80
        y: 5
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40
        horizontalAlignment: TextInput.AlignHCenter

        background: Rectangle {
            radius: 20
            border.color: "lightgray"
            border.width: 0.5
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
        y: sendButton.y - 20 - height
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
        id: sendConfirmation

        title: "Dark Transaction Confirmation"
        text: ""

        standardButtons: StandardButton.Cancel | StandardButton.Yes

        onYes: {
            console.log("Sending Transaction")
            messageDialog.visible = false

            startBusyIndicatorAndDisable();
            sendDarkTransaction(walletShortName)
        }

        onRejected: {
            console.log("Cancel")
            warningLabel.text = "Transaction was canceled!"
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

            if (sendAmountTextField.text === "" || addressTextField.text === "" || emailAddressTextField.text === "") {
                warningLabel.text = "Please complete fields"
            } else {
                startBusyIndicatorAndDisable();
                prepareDarkTransaction(walletShortName);
            }
        }

        contentItem: Text {
            color: "black"
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
            border.color: "black"
            border.width: 1

            color: "white"
        }

        onDownChanged: {
            if (down) {
                contentItem.font.pointSize += 5
                background.border.width += 1
            } else {
                contentItem.font.pointSize -= 5
                background.border.width -= 1
            }
        }
    }
}
