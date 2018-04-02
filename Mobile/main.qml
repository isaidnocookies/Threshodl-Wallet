import QtQuick 2.10
import QtQuick.Window 2.10
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

import com.threebx.walletapp 1.0

Window {
    id: mainWindow
    visible: true
    width: 800
    height: 400
    title: walletApp.title

    WalletApp {
        id: walletApp
    }

    GridLayout {
        id: mainLayout
        columns: 1
        anchors.fill: parent
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

        ListView {
            id: walletList
            width: 200
            height: 100
            model: walletApp.wallets
            delegate: Rectangle {
                width: 200
                height: 25
                Text {
                    text: "Owner: " + model.modelData.owner + ", " + model.modelData.amount + " " + model.modelData.currency + ". Bills: " + model.modelData.count
                }
                ListView {
                    width: 200
                    height: 100
                    model: model.modelData.bills
                    delegate: Rectangle {
                        width: 200
                        height: 25
                        Text {
                            text: "Address: " + model
                        }
                    }
                }
            }
        }
    }

    function updateTitle(title) {
        console.log(walletApp.wallets[0].bills[0].address, walletApp.wallets[0].bills[0].publicKey, walletApp.wallets[0].bills[0].amount, walletApp.wallets[0].bills[0].currency)
        walletApp.title = title;
    }
}
