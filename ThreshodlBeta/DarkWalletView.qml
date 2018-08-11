import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3

Item {
    id: brightWalletViewPage

    property string walletShortName
    property string walletIconPath

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
    }

    Image {
        id: topBarIcon
        source: Qt.resolvedUrl(walletIconPath)

        width: 60
        height: 60

        y: backButton.y
        x: parent.width/2 - width/2
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

//    Button {
//        id: refresh
//        height: 30
//        width: 30

//        background: Rectangle {
//            color: "white"
//            width: parent.height
//            height: parent.width
//            anchors.centerIn: parent
//        }

//        anchors.top: topBarSpacer.bottom
//        x: parent.width - 25 - 30

//        Image {
//            source: "images/assets/refreshButtonIcon.png"
//            fillMode: Image.PreserveAspectFit
//            width: parent.width
//        }

//        onClicked: {
//            // refresh or something...
//        }
//    }

    Text {
        id: totalCurrencyForWallet
        z:10

        font.pointSize: 60
        wrapMode: Text.NoWrap

        font.weight: Font.ExtraLight

        text: getCurrencySymbol("USD") + userAccount.getTotalBalance("USD")
        y: topBarIcon.y + topBarIcon.height + 20
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                if (shortname == walletShortName) {
                    totalCurrencyLabel.text = getCurrencySymbol("USD") + userAccount.getTotalBalance()
                }
            }
        }
    }

    Text {
        id: totalCryptoForWallet
        z:10

        font.bold: true
        font.pointSize: 20

        anchors.top: totalCurrencyForWallet.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "0.0 " + walletShortName
    }

    Text {
        id: walletConfirmationStatusLabel
        z:10

        font.pointSize: 13
        y: totalCryptoForWallet.y + totalCryptoForWallet.height + 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: "(Confirmed)"
    }

    Text {
        id: walletMarketValue
        z:10

        font.pointSize: 13

        y: walletConfirmationStatusLabel.y + walletConfirmationStatusLabel.height + 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: "(Market Value: $10,000)"
    }

    Rectangle {
        id: aboveLineSpacer
        color: "white"
        anchors.top: walletMarketValue.bottom
        height: 30
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
    }

    TabBar {
        id: miniNavBar
        width: parent.width
        height: 70

        anchors.top: aboveLineSpacer.bottom

        ItemDelegate {
            id: tabButtonDelegate
            font.bold: true
        }

        TabButton {
            id: sendTabButton
            text: "Send"

            contentItem: Text {
                color: "black"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: sendTabButton.text
                font.bold: true
                font.pointSize: 13
            }

            background: Rectangle {
                anchors.fill: parent
                Rectangle {
                    visible: miniNavBar.currentIndex == 0 ? true : false
                    color: "#000000"
                    height: 2
                    anchors.bottom: parent.bottom
                    width: parent.width * 0.8
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }

        TabButton {
            id: recieveTabButton
            text: "Recieve"

            contentItem: Text {
                color: "black"
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignHCenter
                text: recieveTabButton.text
                font.bold: true
                font.pointSize: 12
            }

            background: Rectangle {
                color: "white"

                Rectangle {
                    visible: miniNavBar.currentIndex == 1 ? true : false
                    color: "#000000"
                    height: 2
                    anchors.bottom: parent.bottom
                    width: parent.width * 0.8
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
    }

    StackLayout {
        width: parent.width
        anchors.top: miniNavBar.bottom
        anchors.bottom: bottomBarCorrectionSpacer.bottom

        currentIndex: miniNavBar.currentIndex

        Item {
            id: sendTab

            TextField {
                id: sendAmountTextField
                placeholderText: "Amount to Send"
                width: parent.width * 0.80
                y: 20
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                horizontalAlignment: TextInput.AlignHCenter

                background: Rectangle {
                    radius: 20
                    border.color: "lightgray"
                    border.width: 0.5
                }

                onFocusChanged: {
                }
            }

            TextField {
                id: addressTextField
                placeholderText: "To Address"
                width: parent.width * 0.80
                y: sendAmountTextField.y + sendAmountTextField.height + 20
                anchors.horizontalCenter: parent.horizontalCenter
                height: 40
                horizontalAlignment: TextInput.AlignHCenter

                background: Rectangle {
                    radius: 20
                    border.color: "lightgray"
                    border.width: 0.5
                }

                onFocusChanged: {
                }
            }

            Button {
                id: sendButton
                text: "Send"
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width * 0.8
                height: 40
                y: parent.height - bottomAreaCorrectionHeight - height

                onClicked: {
                    //do some stuffs
                }

                contentItem: Text {
                    color: "white"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: sendButton.text
                    font.bold: true
                    font.pointSize: buttonFontSize
                }

                background: Rectangle {
                    width: parent.height
                    height: parent.width
                    anchors.centerIn: parent
                    rotation: 270
                    radius: 20
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#000"
                        }
                        GradientStop {
                            position: 1
                            color: "#555"
                        }
                    }
                }
            }
        }
        Item {
            id: receiveTab

            Image{
                id: qrCodeForRecieve
                source: "image://QZXing/encode/" + "testing qr shit";
                cache: false;
                width: parent.width * 0.5
                height: width

                anchors.horizontalCenter: parent.horizontalCenter
                y: 20
            }

            Text {
                id: qrAddressLabel
                text: "87uftygihojpuygift7oyguhpijohuo8gy7ft"
                wrapMode: Text.WrapAnywhere
                font.pointSize: 13
                font.weight: Font.Thin

                y: qrCodeForRecieve.y + qrCodeForRecieve.height + 10
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
        Item {
            id: transactionsTab
        }
    }
}
