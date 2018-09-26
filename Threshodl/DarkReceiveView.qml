import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

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
        width: parent.width * 0.45
        height: width

        anchors.horizontalCenter: parent.horizontalCenter
        y: 0
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
                    color: "#9b0014"
                }
                GradientStop {
                    position: 1
                    color: "#cc001b"
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
