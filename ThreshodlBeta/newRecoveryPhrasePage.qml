import QtQuick 2.0

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: newRecoveryPhrasePage

    property bool isPasscodeConfirmed: false

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

        enabled: false
        visible: false

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
        text: "Save your recovery phrase"
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
        text:
            "<p>Below is a recovery phrase that will be used to restore your wallets. <b>PLEASE</b> write this down in a safe place. Without these words, you will not be able to recover your wallets or account. These can not be changed once your wallets have been created.</p>"

        lineHeight: 1.5

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: textSpacer.bottom
        width: parent.width * 0.9
        wrapMode: Text.WordWrap
        font.pointSize: 13

        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: fieldSpacer
        width: parent.width
        height: 50
        anchors.top: pageDescription.bottom
        color: "white"
    }

    Rectangle {
        id: firstLine
        width: parent.width * 0.80
        anchors.top: fieldSpacer.bottom
        anchors.right: parent.right
        height: 1

        color: "black"
    }

    Rectangle {
        id: firstLineSpacer2
        width: parent.width
        height: 20
        anchors.top: firstLine.bottom
        color: "white"
    }

    Text {
        id: recoveryPhraseLabel
        color: "black"
        anchors.top: firstLineSpacer2.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.70
        wrapMode: Text.WordWrap
        text: userAccount.recoverySeed

        lineHeight: 1.5
        font.bold: true
        font.pointSize: 16
        horizontalAlignment: Text.AlignHCenter
    }

    Rectangle {
        id: firstLineSpacer3
        width: parent.width
        height: 20
        anchors.top: recoveryPhraseLabel.bottom
        color: "white"
    }

    Rectangle {
        id: secondLine
        width: parent.width * 0.80
        anchors.top: firstLineSpacer3.bottom
        anchors.left: parent.left
        height: 1

        color: "black"
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    BusyIndicator {
        id: mBusyIndicator
        running: false
        anchors.centerIn: parent
    }

    CheckBox {
        id: confirmRecoverySeed
        text: qsTr("I have copied down the recovery words!")
        checked: false

        width: parent.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
        y: continueButton.y - height - 20

        onCheckedChanged: {
            if (confirmRecoverySeed.checked) {
                continueButton.enabled = true
                continueButton.opacity = 1
            } else {
                continueButton.enabled = false
                continueButton.opacity = 0.5
            }
        }
    }

    LockScreen {
        id: passcodeScreen
        setupPasscode: true
        allowBack: false
        visible: false
        screenTitle: "Please create a passcode"

        onPasscodeComplete: {
            if (isPasscodeConfirmed) {
                if (passcodeScreen.passcode === userAccount.getTempPasscode()) {
                    userAccount.confirmPasscodeChange()
                    passcodeScreen.passcode = ""
                    ourStackView.replace(Qt.resolvedUrl("DashboardPage.qml"))
                    console.log("Passcode complete!")
                } else {
                    console.log("Passcode process reset")
                    isPasscodeConfirmed = false
                    allowBack = false
                    screenTitle = "Please enter a passcode"
                    passcodeScreen.passcode = ""
                    ourStackView.replace(passcodeScreen)
                }
            } else {
                console.log("Passcode now confirmed")
                allowBack = false
                userAccount.changePasscode(passcodeScreen.passcode)
                passcodeScreen.passcode = ""
                screenTitle = "Please confirm your passcode"
                isPasscodeConfirmed = true
                ourStackView.replace(passcodeScreen)
            }
        }
    }

    Button {
        id: continueButton
        text: "Continue"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width / 1.5
        height: 40
        opacity: 0.5

        enabled: false

        onClicked: {
            isPasscodeConfirmed = false
            passcodeScreen.visible = true
            ourStackView.push(passcodeScreen)
            console.log("Passcode clicked(1)")
        }

        contentItem: Text {
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: continueButton.text
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
        continueButton.enabled = false
        mBusyIndicator.running = true
    }

    function stopBusyIndicatorAndEnable() {
        backButton.enabled = true
        continueButton.enabled = true
        mBusyIndicator.running = false
    }
}
