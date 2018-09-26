import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: dashboardPage

    Rectangle {
        z:0
        anchors.fill: parent
        Image {
            source: "qrc:/images/assets/appBackgroundForDashboard.jpg"
            fillMode: Image.PreserveAspectFit
            anchors.centerIn: parent
            height: parent.height
        }

//        gradient: Gradient {
//            GradientStop {
//                position: 1
//                color: "#000000"
//            }
//            GradientStop {
//                position: 0
//                color: "#333"
//            }
//        }
    }

    function getCurrencySymbol(iCurrency) {
        if (iCurrency === "USD") {
            return "$"
        } else {
            return ""
        }
    }

    LockScreen {
        id: passcodeLockScreen

        visible: false

        onPasscodeComplete: {
            console.log("UNLOCKED!")
            ourStackView.replace(Qt.resolvedUrl("settingsPage.qml"))
        }
    }

    Rectangle {
        id: topBarSpacer
        color: "transparent"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
        z:8
    }

    Text {
        id: titleLabel
        font.bold: true
        text: titleName
        y: inboxButton.y
        x: (parent.width / 2) - (width / 2)
        z:10
        color: "white"
    }

    Text {
        id: usernameLabel
        text: "Hello! " + userAccount.username
        y: titleLabel.y + titleLabel.height + 10
        x: (parent.width / 2) - (width / 2)
        z:10
        color: "white"
    }

    Button {
        id: inboxButton
        height: 30
        width: 30
        z:10

        visible: false
        enabled: false

        background: Rectangle {
            color: "transparent"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: 25

        Image {
            source: "images/assets/mailButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        onClicked: {
            //onclick: do the mailbox thingy
        }
    }

    Button {
        id: settingsButton
        height: 30
        width: 30
        z:100

        background: Rectangle {
            color: "transparent"
            width: parent.height
            height: parent.width
            anchors.centerIn: parent
        }

        anchors.top: topBarSpacer.bottom
        x: parent.width - width - 25

        Image {
            source: "images/assets/settingsButtonIcon.png"
            fillMode: Image.PreserveAspectFit
            width: parent.width
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                console.log("CLicked settings");
                ourStackView.push(Qt.resolvedUrl("settingsPage.qml"))
            }
        }

    }

    Text {
        id: totalCurrencyLabel
        z:10
        color: "white"

        fontSizeMode: Text.Fit

        font.pointSize: 55
        wrapMode: Text.NoWrap

        font.weight: Font.Thin

        text: getCurrencySymbol("USD") + threshodlTools.formatMarketValueString(userAccount.getTotalBalance())
        y: usernameLabel.y + usernameLabel.height + 50
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: userAccount

            onWalletBalanceUpdateComplete: {
                totalCurrencyLabel.text = getCurrencySymbol("USD") + threshodlTools.formatMarketValueString(userAccount.getTotalBalance())
                totalCurrencyLabel.checkFont()
            }
        }

        function checkFont() {
            if (currencyTypeLabel.x + currencyTypeLabel.width + 5 > parent.width) {
                totalCurrencyLabel.font.pointSize = 30
            } else {
                totalCurrencyLabel.font.pointSize = 55
            }
        }
    }

    Text {
        id:currencyTypeLabel
        z:10

        visible: false
        color: "white"

        text: "USD"

        font.pointSize: 11
        anchors.left: totalCurrencyLabel.right
        y: totalCurrencyLabel.y + totalCurrencyLabel.height * 0.6
    }

    Rectangle {
        id: totalToListSpacer
        anchors.top: totalCurrencyLabel.bottom
        height: 20
        width: parent.width
        z:8
        color: "transparent"
    }

    Rectangle {
        z:8
        id: listMask
        color: "transparent"
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: totalToListSpacer.bottom
    }

    Rectangle {
        id: topShadowEffect
        anchors.top: listMask.bottom
        height: 8
        width: parent.width
        z:9
        opacity: 0.4
        color: "transparent"
    }

    ListView {
        z:50
        id: cryptoWallets
        model: DashboardListModel {}
        delegate: CryptoWalletDelegate{}

        spacing: 8

        header: Component {
            Rectangle {
                height: 250
                color: "transparent"
                width: parent.parent.width;
            }
        }

        width: parent.width * cellWidthPercent
        x: parent.width * (1 - cellWidthPercent)
        anchors.top: parent.top
        anchors.bottom: bottomBarCorrectionSpacer.top
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "transparent"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
        z:10
    }
}
