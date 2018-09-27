import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

import "qrc:/darkWalletView/";

Item {
    id: darkWalletDepositPage

    property string shortname
    property string amount
    property string notes

    function setText() {
        title.text = "Dark " + shortname + " Import"
        descriptionLabel.text = "You are importing coins into your Dark account. This will add coin directly into your " + shortname + " wallet. Once they have been imported, they will be visible within the associated dark wallet. For additional information related to dark coins / assets, please visit www.threshodl.com or contact us at support@threshodl.com"
        amountLabel.text = amount + " " + shortname

        if (notes === "") {
            noteLabel.text = " (No notes...)"
        } else {
            noteLabel.text = notes
        }
    }

    MessageDialog {
        id: messageDialog

        title: "Dark Wallet Import"
        text: ""

        standardButtons: StandardButton.Ok

        onAccepted: {
            console.log("Import Complete.")
            messageDialog.visible = false;
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
                position: 1
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
        source: "qrc:/images/assets/cryptoWalletIcon.png"

        width: 60
        height: 60

        anchors.top: topBarSpacer.bottom
        x: parent.width/2 - width/2
    }

    Text {
        id: title
        y: topBarIcon.y + (topBarIcon.height) + 30
        anchors.horizontalCenter: parent.horizontalCenter
        text: ""
        font.pointSize: 28

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
        id: amountLabel
        y: bottomBackgroundShape.y + 20
        width: parent.width * 0.80

        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 18
    }

    Text {
        id: descriptionLabel
        anchors.top: amountLabel.bottom
        anchors.topMargin: 10

        width: parent.width * 0.80
        text: ""
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 15
    }

    Text {
        id: noteLabel
        anchors.top: descriptionLabel.bottom
        anchors.topMargin: 10

        width: parent.width * 0.80
        text: ""
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 15
    }

    Button {
        id: importButton
        text: "Import Wallets"
        width: parent.width * 0.85
        height: 40
        anchors.bottom: cancelButton.top
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter

        onDownChanged: {
            if (importButton.down === true) {
                importButtonText.color = "darkgray"
            } else {
                importButtonText.color = "white"
            }
        }

        onClicked: {
            console.log("Go forth and import the wallets!")
            var importSuccess = userAccount.importWallets()
            if (importSuccess) {
                messageDialog.text = "Import Successful!"
                messageDialog.open()
            } else {
                messageDialog.text = "Import Failed! Please try again"
                messageDialog.open()
            }
            ourStackView.pop()
        }

        contentItem: Text {
            id: importButtonText
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: importButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            id: importButtonBackground
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

    Button {
        id: cancelButton
        text: "Cancel Import"
        width: parent.width * 0.85
        height: 40
        y: parent.height - bottomAreaCorrectionHeight - height
        anchors.horizontalCenter: parent.horizontalCenter

        onDownChanged: {
            if (cancelButton.down === true) {
                cancelButtonText.color = "darkgray"
            } else {
                cancelButtonText.color = "white"
            }
        }

        onClicked: {
            ourStackView.pop();
        }

        contentItem: Text {
            id: cancelButtonText
            color: "darkgray"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: cancelButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            id: cancelButtonBackground
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: 20
            border.color: "darkgray"
            color: "transparent"
        }
    }
}
