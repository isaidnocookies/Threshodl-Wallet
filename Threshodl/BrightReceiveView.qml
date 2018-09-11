import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

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
            source: "qrc:/images/assets/clipboardPasteIcon.png"
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
