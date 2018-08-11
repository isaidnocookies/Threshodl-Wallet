import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: recoveryRecoverySeed

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
    }

    Text {
        id: title
        text: titleName
        font.bold: true
        y: backButton.y + backButton.height / 3
        x: (parent.width / 2) - (width / 2)
    }

    Button {
        id: backButton
        height: 30
        width: 30

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

    Text {
        id: pageTitleDescription
        text: "Please enter your recovery seed"
        color: "black"
        font.bold: true
        font.pointSize: 15

        anchors.horizontalCenter: parent.horizontalCenter
        y: title.x + 30
    }

    Rectangle {
        id: textSpacer
        width: parent.width
        height: 10
        anchors.top: pageTitleDescription.bottom
        color: "white"
    }

    Text {
        id: pageDescription
        text: "Enter the recovery seed you were given when you first created your account. This will allow your wallets to be restored and act as a key for your backup file"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: textSpacer.bottom
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font.pointSize: 13

        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: textFieldSpacer
        width: parent.width
        height: 40
        anchors.top: pageDescription.bottom
        color: "white"
    }

    TextArea {
        id: recoverySeedField
        placeholderText: "Recovery Seed"
        width: parent.width / 1.5
        anchors.top: textFieldSpacer.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        height: 150
        z:10
        horizontalAlignment: TextInput.AlignHCenter

        wrapMode: TextEdit.WordWrap
        inputMethodHints: Qt.ImhPreferLowercase

        font.bold: true
        font.pointSize: 16
        verticalAlignment: TextEdit.AlignVCenter

        background: Rectangle {
            radius: 20
            border.color: "lightgray"
            border.width: 0.5
        }

        onFocusChanged: {
            warningLabel.text = ""
        }

        Keys.onReturnPressed: {
            Qt.inputMethod.hide()
        }
    }

    Rectangle {
        id: textFieldSpacer2
        width: parent.width
        height: 20
        anchors.top: recoverySeedField.bottom
        color: "white"
    }

    Text {
        id: warningLabel
        color: "red"
        anchors.top: textFieldSpacer2.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        text: ""

        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
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


    Button {
        id: recoveryAccountButton
        text: "Continue"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width / 1.5
        height: 40
        z: 50

        onClicked: {
            var lSeed = recoverySeedField.text
            lSeed = lSeed.toLowerCase()

            if (lSeed.length <= 0) {
                warningLabel.text = "Please input username"
            } else {
                startBusyIndicatorAndDisable()
                console.log("Starting to recover account");
                console.log(lSeed)

                userAccountSignalConnection.enabled = true
                userAccount.recoverAccount(lSeed)
            }
        }

        Connections {
            id: userAccountSignalConnection
            target: userAccount
            enabled: false
            onUserCreationFinished: {
                stopBusyIndicatorAndEnable()
                console.log("Account Recovery Complete : Message from createUsernamePage.qml")

                if (success) {
                    console.log("Recover Account Successful");
                    userAccountSignalConnection.enabled = false
                    ourStackView.push(Qt.resolvedUrl("dashboardPage.qml"))
                } else {
                    console.log("Recover Account Failed");
                    userAccountSignalConnection.enabled = false
                    warningLabel.text = error
                }
            }
        }

        contentItem: Text {
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: recoveryAccountButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        anchors.bottom: bottomBarCorrectionSpacer.top

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

    function startBusyIndicatorAndDisable() {
        backButton.enabled = false
        recoverySeedField.enabled = false
        recoveryAccountButton.enabled = false
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        backButton.enabled = true
        recoverySeedField.enabled = true
        recoveryAccountButton.enabled = true
        mWaitingLayer.visible = false
    }
}
