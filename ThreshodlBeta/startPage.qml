import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: createPageView

    Image {
        source: "images/assets/threshodlLogo_landing.png"
        fillMode: Image.PreserveAspectFit

        width: parent.width
        y: parent.height / 3.5
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    Rectangle {
        id: buttonSpacer
        anchors.bottom: recoverWalletButton.top
        height: 10
    }

    Button {
        id: createNewWalletButton
        text: "Create New Wallet"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width / 1.5
        height: 40

        contentItem: Text {
            color: "white"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: createNewWalletButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        anchors.bottom: buttonSpacer.top

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

        onClicked: ourStackView.push(Qt.resolvedUrl("createUsernamePage.qml"))
    }

    Button {
        id: recoverWalletButton
        text: "Recover Existing Wallet"
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width / 1.5
        height: 40

        anchors.bottom: bottomBarCorrectionSpacer.top

        contentItem: Text {
            color: "#6796FF"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            text: recoverWalletButton.text
            font.bold: true
            font.pointSize: buttonFontSize
        }

        background: Rectangle {
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
            rotation: 270
            radius: 20

            border.width: 2
            border.color: "#6796FF"

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#fff"
                }
                GradientStop {
                    position: 1
                    color: "#fff"
                }
            }
        }
    }
}
