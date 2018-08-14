import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3

Item {
    id: settingsPage

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
    }

    Text {
        id: title
        text: "Settings"
        font.bold: true
        y: backButton.y + backButton.height / 3
        x: (parent.width / 2) - (width / 2)
    }

    Button {
        id: backButton
        height: 30
        width: 30
        z: 2

        background: Rectangle {
            color: "white"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: 25

        Image {
            source: "images/assets/backButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: ourStackView.pop()
    }

    Rectangle {
        id: topTableSpacer
        anchors.top: title.bottom
        width: parent.width
        height: 100
        color: "white"
        z: 0
    }

    Item {
        id: settingOptions
        anchors.top: topTableSpacer.bottom
        width: parent.width
        anchors.bottom: parent.bottom

        // settings stuff

        Column {
            anchors.fill: parent
            spacing: 20

            Button {
                id: resetPasscode
                anchors.horizontalCenter: parent.horizontalCenter

                width: parent.width * 0.7
                height: 50

                text: "Reset Passcode"
                font.bold: true
                font.pointSize: 14

                background: Rectangle {
                    color: "lightgray"
                    radius: 10
                    anchors.fill: parent
                }

                onClicked: {
                    console.log("Let's reset the passcode...")
                }
            }

            Button {
                id: viewPrivateKeys
                anchors.horizontalCenter: parent.horizontalCenter

                width: parent.width * 0.7
                height: 50

                text: "View Private Keys"
                font.bold: true
                font.pointSize: 14

                background: Rectangle {
                    color: "lightgray"
                    radius: 10
                    anchors.fill: parent
                }

                onClicked: {
                    console.log("View Private Keys Button...")
                    ourStackView.push(Qt.resolvedUrl("ViewPrivateKeysPage.qml"))
                }
            }

            MessageDialog {
                id: messageAlert
                standardButtons: StandardButton.Cancel | StandardButton.Ok

                onRejected: {
                    console.log("ABORT")
                }

                onAccepted:  {
                    console.log("Lets do it...")
                }
            }

            Button {
                id: clearAllLocalData
                anchors.horizontalCenter: parent.horizontalCenter

                width: parent.width * 0.7
                height: 50

                text: "Clear All Local Data"
                font.bold: true
                font.pointSize: 14

                background: Rectangle {
                    color: "lightgray"
                    radius: 10
                    anchors.fill: parent
                }

                onClicked: {
                    messageAlert.title = "Are you sure?"
                    messageAlert.detailedText = "Are you sure you want to clear all local data? To recover the data, you will have to restore from your recovery seed and use a backup file for upcoming dark wallets."
                    messageAlert.open()
                }
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
    }

    function startBusyIndicatorAndDisable() {
        backButton.enabled = false
        usernameField.enabled = false
        createNewWalletButton.enabled = false
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        backButton.enabled = true
        usernameField.enabled = true
        createNewWalletButton.enabled = true
        mWaitingLayer.visible = false
    }
}
