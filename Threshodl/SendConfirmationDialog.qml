import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Dialog {
    property string toAddress : ""
    property string toAmount : ""

    property real parentHeight : 100.00
    property real parentWidth : 100.00

    id: messageDialog
    title: "Confirm Transaction"

    visible: false

    height: parent.height * 0.65
    width: parent.width * 0.85

    contentItem: Rectangle {
        implicitHeight: parentHeight * 0.65
        implicitWidth: parentWidth * 0.85

        radius: 30
        border.color: "black"
        border.width: 1

        Text {
            id: dialogTitleText
            text: "Transaction Confirmation"

            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.y + 20
        }

        Text {
            id: dialogBodyText
            text: "Are you sure you want to send this transaction? Blockchain transactions are not reversable once submitted to the network"
            anchors.horizontalCenter: parent.horizontalCenter

            width: parent.width * 0.9
            y: dialogTitleText.y + dialogTitleText.height + 20

            wrapMode: Text.WordWrap
        }

        Text {
            id: toAddressLabelHeading
            text: "Address"
            width: parent.width * 0.9
            y: dialogBodyText.y + dialogBodyText.height + 40
        }

        Text {
            id: toAddressLabel
            text: toAddress
            width: parent.width * 0.9
            wrapMode: Text.WrapAnywhere
            y: toAddressLabelHeading.y + toAddressLabelHeading.height + 20
        }

        Button {
            text: "Cancel"
            anchors.horizontalCenter: parent.horizontalCenter
            y: dialogTitleText.y + dialogTitleText.height + 10

            onClicked: {
                messageDialog.close()
            }
        }

    }
}
