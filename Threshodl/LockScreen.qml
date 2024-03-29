import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Item {
    id: lockScreen
    property bool allowBack: true
    property int digitIndex: 0
    property string passcode: ""
    property bool setupPasscode: false

    property alias screenTitle: pageTitleDescription.text
    property alias screenWarning: pageWarning.text

    signal passcodeComplete()

    Rectangle {
        id: topBarSpacer
        color: "transparent"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
    }

    Rectangle {
        id: lockScreenBackground
        color: "black"
        anchors.fill: parent
    }

//    Text {
//        id: title
//        text: "threshodl"
//        font.bold: false
//        y: backButton.y + backButton.height / 3
//        x: (parent.width / 2) - (width / 2)
//    }

    Image {
        id: title
        source: "images/assets/threshodlLockIcon.png"
        height: backButton.height * 2
        width: height
        y: backButton.y + backButton.height / 3
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Button {
        id: backButton
        height: 30
        width: 30

        visible: allowBack
        enabled: allowBack

        anchors.top: topBarSpacer.bottom
        x: 25

        background: Rectangle {
            color: "transparent"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        Image {
            source: "images/assets/whiteBackButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: {
            passcode = ""
            digitIndex = 0
            ourStackView.pop()
        }
    }

    Text {
        id: pageTitleDescription
        text: "Please enter your passcode"
        color: "white"
        font.bold: false
        font.pointSize: 16

        anchors.horizontalCenter: parent.horizontalCenter
        y: title.y + title.height + 40
    }

    Text {
        id: pageWarning
        text: ""
        color: "white"
        font.pointSize: 14

        width: parent.width * 0.8
        y: pageTitleDescription.y + pageTitleDescription.height + 10
    }

    GridLayout {

        id: digitInputBubbles
        width: parent.width * 0.7
        columns: 4
        columnSpacing: (parent.width - 80) / 5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: pageTitleDescription.bottom
        anchors.bottom: bubbleNumPad.top

        Rectangle {
            id: digit0; height: 20; width: 20; radius: width * 0.5
            border.color: { if (isDigitCompleted(0)) { "white" } else { "white" } }
            color: { if (isDigitCompleted(0)) { "white" } else { "transparent" } }
        }

        Rectangle {
            id: digit1; height: 20; width: 20; radius: width * 0.5
            border.color: { if (isDigitCompleted(1)) { "white" } else { "white" } }
            color: { if (isDigitCompleted(1)) { "white" } else { "transparent" } }
        }

        Rectangle {
            id: digit2; height: 20; width: 20; radius: width * 0.5
            border.color: { if (isDigitCompleted(2)) { "white" } else { "white" } }
            color: { if (isDigitCompleted(2)) { "white" } else { "transparent" } }
        }

        Rectangle {
            id: digit3; height: 20; width: 20; radius: width * 0.5
            border.color: { if (isDigitCompleted(3)) { "white" } else { "white" } }
            color: { if (isDigitCompleted(3)) { "white" } else { "transparent" } }
        }
    }

    Rectangle {
        id: bottomSpacer
        width: parent.width
        height: 60
        anchors.bottom: parent.bottom
        color: "transparent"
    }

    GridLayout {
        id: bubbleNumPad
        width: parent.width * 0.8
        height: parent.height / 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: bottomSpacer.top

        Layout.alignment: Qt.AlignHCenter
        columnSpacing: 30
        columns: 3

        Repeater {
            model: 12

            Item {
                id: buttonItem

                Layout.fillHeight: true
                Layout.fillWidth: true

                Rectangle {
                    id: buttonShape
                    width: parent.width
                    height: width
                    color: "transparent"
                    border.color: getButtonColor(index)
                    border.width: 1

                    radius: height/2
                    opacity: (index === 9 || index === 11) ? 0 : 1

                    Text {
                        id: buttonText
                        text: getDigitValue(index)
                        color: getButtonColor(index)
                        font.bold: false
                        font.pointSize: 18
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    id: buttonMouseArea
                    anchors.fill: buttonShape
                    enabled: (index === 9 || index === 11) ? false : true

                    onPressed: {
                        buttonShape.color = "lightgray"
                    }

                    onReleased: {
                        buttonShape.color = "transparent"
                    }

                    onClicked: {
                        passcode = passcode + getDigitValue(index)
                        digitIndex++

                        if (digitIndex === 4 && setupPasscode) {
                            digitIndex = 0
                            passcodeComplete()
                        } else {
                            if (userAccount.checkPasscode(passcode)) {
                                console.log("Success!")
                                passcode = ""
                                digitIndex = 0
                                passcodeComplete()

                            } else if (digitIndex === 4 && !userAccount.checkPasscode(passcode)){
                                console.log("Failed!")
                                passcode = ""
                                digitIndex = 0
                            }
                        }
                    }
                }
            }
        }
    }

    function getButtonColor(index) {
        return "white"
    }

    function getDigitValue(index) {
        if (index === 10) {
            return 0;
        } else {
            return (index + 1) % 10;
        }
    }

    function isDigitCompleted(index) {
        if (index < digitIndex) {
            return true;
        }
        return false;
    }
}
