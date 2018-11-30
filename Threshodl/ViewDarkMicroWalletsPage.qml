import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3

Item {
    id: darkMicroWalletsPage

    property var shortName
    property var walletList

    function populateTable() {
        console.log("Getting wallets for ... " + shortName);
        walletModel.clear();
        walletList = userAccount.getDarkWallets(shortName);
        for (var i = 0; i < walletList.length; i = i + 3) {
            walletModel.append({ value: walletList[i], address: walletList[i+1], privateKey: walletList[i+2] });
        }
    }

    Rectangle {
        id: topBackground
        anchors.top: parent.top
        width: parent.width
        height: parent.height / 3

        color: "black"
    }

    Rectangle {
        id: topBarSpacer
        color: "transparent"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
        z: 5
    }

    Text {
        id: title
        text: "Microwallets"
        y: backButton.y + (backButton.height / 2) - (height/2)
        x: (parent.width / 2) - (width / 2)
        z: 5

        font.pointSize: 14
        font.bold: true
        color: "white"
    }

    Rectangle {
        id: bottomBackground
        width: parent.width
        radius: 25
        color: "white"
        anchors.bottom: parent.bottom
        anchors.top: titleSpacer.bottom
        anchors.topMargin: 20
    }

    Rectangle {
        id: titleSpacer
        color: "transparent"
        width: parent.width
        height: 20
        anchors.top: title.bottom
        z: 5
    }

    Rectangle {
        id: headerBackground
        color: "transparent"
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


    ListModel {
        id: walletModel
    }

    Component {
        id: walletModelDelegate

        Item {
            width: parent.width
            height: (rowLine.y + rowLine.height) - valueOfMicroWallet.y + 20

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
                y: valueOfMicroWallet.y + valueOfMicroWallet.height + 10
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
                detailedText: "The wallet address has been copied to your clipboard"
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    copyAlert.open()
                    threshodlTools.copyToClipboard(address)
                }
            }
        }
    }

    ListView {
        id: walletListView
        model: walletModel
        anchors.top: bottomBackground.top
        anchors.topMargin: 45
        anchors.bottom: bottomBarCorrectionSpacer.top
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter
        delegate: walletModelDelegate

        z: 0
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "transparent"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }
}
