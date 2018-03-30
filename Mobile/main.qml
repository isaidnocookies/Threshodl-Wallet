import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import com.threebx.walletapp 1.0

Window {
    id: mainWindow
    visible: true
    width: 400
    height: 600
    title: walletApp.title

    WalletApp {
        id: walletApp
    }

    GridLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: margin
        Button {
            id: button1
            text: "Click to change title"
            onClicked: updateTitle("This is new!")
        }

        Button {
            id: button2
            text: "Click to send a parammeter"
            onClicked: walletApp.listener("Parameter1", 234)
        }
    }

    function updateTitle(title) {
        walletApp.title = title;
        mainWindow.title = walletApp.title;
    }
}
