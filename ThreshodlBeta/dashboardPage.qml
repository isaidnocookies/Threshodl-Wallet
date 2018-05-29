import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Item {
    id: dashboardPage

    function getCurrencySymbol(iCurrency) {
        if (iCurrency === "USD") {
            return "$"
        } else {
            return ""
        }
    }

    Rectangle {
        id: topBarSpacer
        color: "white"
        anchors.top: parent.top
        width: parent.width
        height: topAreaCorrectionHeight
        z:10
    }

    Text {
        id: titleLabel
        font.bold: true
        text: titleName
        y: inboxButton.y
        x: (parent.width / 2) - (width / 2)
        z:10
    }

    Text {
        id: usernameLabel
        text: "Hello, " + userAccount.username
        y: titleLabel.y + titleLabel.height + 5
        x: (parent.width / 2) - (width / 2)
        z:10
    }


    Button {
        id: inboxButton
        height: 30
        width: 30
        z:10

        visible: false
        enabled: false

        background: Rectangle {
            color: "white"
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
        z:10

        background: Rectangle {
            color: "white"
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

        //onclick: do the settings thingy
    }

    Text {
        id: totalCurrencyLabel
        z:10

        font.pointSize: 60
        wrapMode: Text.NoWrap

        font.weight: Font.ExtraLight

        text: getCurrencySymbol("USD") + userAccount.getTotalBalance("USD")
        y: usernameLabel.y + usernameLabel.height + 80
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: userAccount
            onCryptoConfirmedBalanceChanged: {
                totalCurrencyLabel.text = getCurrencySymbol("USD") + userAccount.getTotalBalance()
            }
        }
    }

    Text {
        id:currencyTypeLabel
        z:10

        text: "USD"

        font.pointSize: 11
        anchors.left: totalCurrencyLabel.right
        y: totalCurrencyLabel.y + totalCurrencyLabel.height * 0.6
    }

    Rectangle {
        id: totalToListSpacer
        anchors.top: totalCurrencyLabel.bottom
        height: 60
        width: parent.width
        z:10
        color: Qt.rgba(1,1,1,0)
    }

    Rectangle {
        z:9
        id: listMask
        color: "white"
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: totalToListSpacer.bottom

        gradient: Gradient {
            GradientStop {
                position: 0.9
                color: Qt.rgba(1, 1, 1, 1)
            }
            GradientStop {
                position: 1
                color: Qt.rgba(1, 1, 1, 0)
            }
        }
    }

    ListView {
        z: 5
        id: cryptoWallets
        model: DashboardListModel {}
        delegate: CryptoWalletDelegate{}

        width: parent.width
        anchors.top: totalToListSpacer.bottom
        anchors.bottom: bottomBarCorrectionSpacer.top
    }

    Rectangle {
        id: bottomBarCorrectionSpacer
        color: "white"
        anchors.bottom: parent.bottom
        height: bottomAreaCorrectionHeight
        width: parent.width
        z:10
    }
}
