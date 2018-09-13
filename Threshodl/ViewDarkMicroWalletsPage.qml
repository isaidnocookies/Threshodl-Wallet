import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3

Item {
    id: darkMicroWalletsPage

    property var shortName
    property var walletList

    Component.onCompleted: {
        walletList = userAccount.getDarkWallets(shortName);
        for (var i = 0; i < walletList.length; i = i + 3) {
            walletModel.append({ value: walletList[i], address: walletList[i+1], privateKey: walletList[i+2] });
        }
    }

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
        z: 5
    }

    Text {
        id: title
        text: "Microwallets"
        font.bold: true
        y: backButton.y + backButton.height / 3
        x: (parent.width / 2) - (width / 2)
        z: 5
    }

    Rectangle {
        id: titleSpacer
        color: "white"
        width: parent.width
        height: 40
        anchors.top: title.bottom
        z: 5
    }

    Rectangle {
        id: headerBackground
        color: "white"
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: titleSpacer.bottom
        z: 2
    }

    Button {
        id: backButton
        height: 30
        width: 30
        z: 5

        background: Rectangle {
            color: "white"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: 25

        Image {
            source: "qrc:/images/assets/backButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: ourStackView.pop()
    }


    ListModel {
        id: walletModel
    }

    Component {
        id: walletModelDelegate

        Item {
            width: parent.width
            height: (rowLine.y + rowLine.height) - coinName.y + 20

            Text {
                id: valueOfMicroWallet
                anchors.top: parent.top
                width: parent.width
                text: value
                color: "black"
                wrapMode: Text.WrapAnywhere
                horizontalAlignment: Text.AlignHCenter

                font.pointSize: 12
            }
            Text {
                id: addressLabel
                text: address
                y: coinName.y + coinName.height + 10
                width: parent.width
                wrapMode: Text.WrapAnywhere
                color: "black"
                horizontalAlignment: Text.AlignHCenter

                font.bold: true
                font.pointSize: 14
            }
            Text {
                id: privateKeyLabel
                text: privateKey
                y: addressLabel.y + addressLabel.height + 10
                width: parent.width
                color: "black"
                wrapMode: Text.WrapAnywhere
                horizontalAlignment: Text.AlignHCenter
                font.pointSize: 10
            }
            Rectangle {
                id: rowLine
                height: 1
                width: parent.width
                y: privateKeyLabel.y + privateKeyLabel.height + 20
                color: "lightgray"
            }

            MessageDialog {
                id: copyAlert
                title: "Copied"
                detailedText: "The wallet details have been copied to your clipboard"
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.log("Clicked row")
                    copyAlert.open()
                    threshodlTools.copyToClipboard(address)
                }
            }
        }
    }

    ListView {
        id: walletListView
        model: walletModel
        anchors.top: titleSpacer.bottom
        anchors.bottom: bottomBarCorrectionSpacer.top
        width: parent.width * 0.9
        anchors.horizontalCenter: parent.horizontalCenter
        delegate: walletModelDelegate

        z: 0
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }
}
