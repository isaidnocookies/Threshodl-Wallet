import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3

Item {
    id: privateKeysPage

    function changeUsername(newUsername) {
        console.log("Changing username....")
        startBusyIndicatorAndDisable();
        userAccount.changeUsername(newUsername);
    }

    function startBusyIndicatorAndDisable() {
        mWaitingLayer.visible = true
    }

    function stopBusyIndicatorAndEnable() {
        mWaitingLayer.visible = false
    }

    Connections {
        target: userAccount;

        // void usernameChangeSuccess(bool success);
        onUsernameChangeSuccess: {
            stopBusyIndicatorAndEnable();
            usernameChangeConfirmation.setMessage(success);
            usernameChangeConfirmation.open();
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
        }
    }

    MessageDialog {
        id: usernameChangeConfirmation

        function setMessage(success) {
            if (success) {
                usernameChangeConfirmation.text = "Username was successfully changed!!"
            } else {
                usernameChangeConfirmation.text = "Request to change username failed! For help, please contact support@threshodl.com"
            }
        }
        title: "Change Username"
        text: ""

        standardButtons: StandardButton.Ok
        onAccepted: {
            console.log("Username confirmation accepted")
            ourStackView.pop();
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
        text: "Change Username"
        font.bold: true
        y: backButton.y + (backButton.height / 2) - (height/2)
        x: (parent.width / 2) - (width / 2)
        z: 5

        font.pointSize: 14

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

    Text {
        id: descriptionLabelHeader
        text: "Current username:";
        anchors.top: bottomBackground.top;
        anchors.topMargin: 50;
        x: (parent.width / 2) - (width / 2)
    }

    Text {
        id: descriptionLabel
        text:userAccount.username;
        anchors.top: descriptionLabelHeader.top;
        anchors.topMargin: 20;
        x: (parent.width / 2) - (width / 2)
        font.bold: true;
        font.pointSize: descriptionLabelHeader.font.pointSize + 2;
    }

    TextField {
        id: newUsernameField
        placeholderText: "New Username"
        width: parent.width * 0.80
        y: descriptionLabel.y + descriptionLabel.height + 20
        anchors.horizontalCenter: parent.horizontalCenter
        height: 40
        horizontalAlignment: TextInput.AlignHCenter

        background: Rectangle {
            radius: 20
            border.color: "lightgray"
            border.width: 0.5
        }
    }

    Button {
        id: changeButton
        text: "Send"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width * 0.8
        height: 40
        y: parent.height - bottomAreaCorrectionHeight - height

        onClicked: {

            if (newUsernameField.text === "") {
                warningLabel.text = "Please complete fields"
            } else {
                startBusyIndicatorAndDisable();
                changeUsername(newUsernameField.text);
            }
        }

        contentItem: Text {
            color: "black"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: changeButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: 20
            border.color: "black"
            border.width: 1

            color: "white"
        }

        onDownChanged: {
            if (down) {
                contentItem.font.pointSize += 5
                background.border.width += 1
            } else {
                contentItem.font.pointSize -= 5
                background.border.width -= 1
            }
        }
    }

    Text {
        id: warningLabel
        y: changeButton .y - 20 - height
        anchors.horizontalCenter: parent.horizontalCenter
        color: "red"
        text: ""

        onTextChanged: {
            console.log("Warning label text changed")
            warningLabelTimer.start()
        }
    }

    Timer {
        id: warningLabelTimer
        repeat: false
        running: false
        interval: 5000

        onTriggered: {
            warningLabel.text = ""
            console.log("Warning text reset")
        }
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "transparent"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }
}
