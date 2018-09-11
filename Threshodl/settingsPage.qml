import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3

import "qrc:/"

Item {
    id: settingsPage

    property bool originalPasscodeConfirmed: false
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
            source: "qrc:/images/assets/backButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: ourStackView.pop()
    }

    Image {
        source: "qrc:/images/assets/settingsButtonIcon.png"
        fillMode: Image.PreserveAspectFit
        width: parent.width / 8
        height: width
        z: 5

        anchors.horizontalCenter: parent.horizontalCenter
        y:title.y + title.height + 25
    }

    Rectangle {
        id: topTableSpacer
        anchors.top: title.bottom
        width: parent.width
        height: 100
        color: "white"
        z: 0
    }

    LockScreen {
        id: passcodeScreen
        allowBack: true
        visible: false

        screenTitle: "Please enter your passcode"
        setupPasscode: false

        onPasscodeComplete: {
            allowBack: true

            if (!originalPasscodeConfirmed) {
                originalPasscodeConfirmed = true
                allowBack = false
                screenTitle = "Enter new passcode"
                setupPasscode = true
                ourStackView.replace(passcodeScreen)
            } else {
                if (isPasscodeConfirmed) {
                    if (passcodeScreen.passcode === userAccount.getTempPasscode()) {
                        userAccount.confirmPasscodeChange()
                        passcodeScreen.passcode = ""
                        ourStackView.pop()
                        allowBack = true
                        setupPasscode = false
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
    }

    Item {
        id: settingOptions
        anchors.top: topTableSpacer.bottom
        width: parent.width
        anchors.bottom: parent.bottom

        // settings stuff

        Column {
            anchors.fill: parent
            spacing: 5

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }

            Row {
                height: 60
                width: parent.width

                Button {
                    id: resetPasscode

                    width: parent.width
                    height: parent.height

                    text: "Reset Passcode"
                    font.bold: true
                    font.pointSize: 14

                    background: Rectangle {
                        color: "white"
                        anchors.fill: parent
                    }

                    onClicked: {
                        console.log("Let's reset the passcode...")
                        passcodeScreen.visible = true
                        ourStackView.push(passcodeScreen)
                    }
                }
            }

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }

            Row {
                height: 60
                width: parent.width

                Button {
                    id: viewPrivateKeys

                    width: parent.width
                    height: parent.height

                    text: "View Private Keys"
                    font.bold: true
                    font.pointSize: 14

                    background: Rectangle {
                        color: "white"
                        anchors.fill: parent
                    }

                    onClicked: {
                        console.log("View Private Keys Button...")
                        ourStackView.push(Qt.resolvedUrl("qrc:/settings/ViewPrivateKeysPage.qml"))
                    }
                }
            }

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }

            Row {
                height: 60
                width: parent.width

                Button {
                    id: viewRecoveryPhrase

                    width: parent.width
                    height: parent.height

                    text: "View Recovery Phrase"
                    font.bold: true
                    font.pointSize: 14

                    background: Rectangle {
                        color: "white"
                        anchors.fill: parent
                    }

                    onClicked: {
                        console.log("View Recovery Phrase...")
                        messageAlert.title = "Recovery Phrase"
                        messageAlert.detailedText = userAccount.getRecoverySeed()
                        messageAlert.open()
                    }
                }
            }

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }

            MessageDialog {
                id: messageAlert
            }

            Row {
                height: 60
                width: parent.width
                Button {
                    id: clearAllLocalData

                    width: parent.width
                    height: parent.height

                    text: "Clear All Local Data"
                    font.bold: true
                    font.pointSize: 14

                    background: Rectangle {
                        color: "white"
                        radius: 20
                        anchors.fill: parent
                    }

                    onClicked: {
                        messageAlert.title = "Reset Application"
                        messageAlert.detailedText = "To reset all data on this device, please uninstall and then re-install the application. Prior to doing this, please make sure you have your recovery phrase and any additional backups!"
                        messageAlert.open()
                    }
                }
            }

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }

            Row {
                height: 60
                width: parent.width
                spacing: 20

                Button {
                    id: contactUs

                    width: parent.width
                    height: parent.height

                    text: "Contact Us"
                    font.bold: true
                    font.pointSize: 14

                    background: Rectangle {
                        color: "white"
                        radius: 20
                        anchors.fill: parent
                    }

//                    Image {
//                        anchors.verticalCenter: parent.verticalCenter
//                        x: parent.width / 4
//                        source : "images/assets/emailIcon.png"
//                        height: parent.height * 0.5
//                        width: height
//                    }

                    onClicked: {
                        Qt.openUrlExternally("mailto:hello@threebx.com?subject=Hello!")
                    }
                }
            }

            Row { height: 1; width: parent.width; leftPadding: (width * 0.15)/2;
                Rectangle { height: parent.height; width: parent.width * 0.85; color: "lightgray"; }
            }
        }
    }
}
