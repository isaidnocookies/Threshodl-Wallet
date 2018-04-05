import QtQuick 2.10
import QtQuick.Controls 2.3

import "."

Page {
    width: 600
    height: 400

    header: Label {
        text: qsTr("Page 1")
        font.pixelSize: Qt.application.font.pixelSize * 1.6
        padding: 10
    }

    Label {
        text: qsTr("You are on Page 1.")
        anchors.centerIn: parent
    }

    ListView {
        id: walletList
        width: 200
        height: 100
        model: WalletApp.wallets

        delegate: Rectangle {
            width: 200
            height: 25
            Text {
                text: "Owner: " + model.modelData.owner + ", " + model.modelData.amount + " "
                      + model.modelData.currency + ". Bills: " + model.modelData.bills.length
            }
        }
    }
}
