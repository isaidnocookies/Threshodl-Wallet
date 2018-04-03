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
            delegate: Component{
                Rectangle {
                    width: 200
                    height: col.height

                    Column{
                        id: col
                        anchors.left: parent.left
                        anchors.top: parent.top
                        height: walletApp.wallets.count*20 + header.height + buttonsRow.height
                        width: parent.width

                        Text{
                            id: header
                            width: parent.width
                            height: 40
                            text: "Owner: " + model.modelData.owner + ", " + model.modelData.amount + " " + model.modelData.currency + ". Bills: " + model.modelData.bills.length
                        }

                        ListView {
                            id: list
                            model: model.modelData.bills
                            width: 0.7*parent.width
                            height: count*20
                            anchors.horizontalCenter: parent.horizontalCenter

                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 20
                                color: itemColor
                                border.color: "gray"
                                border.width: 1

                                MouseArea{
                                    anchors.fill: parent
                                    onClicked: console.log(parent.color)
                                }
                            }
                        }
                    }
                }
            }

            /*delegate: Rectangle {
                width: 200
                height: 100
                Text {
                    text: "Owner: " + model.modelData.owner + ", " + model.modelData.amount + " " + model.modelData.currency + ". Bills: " + model.modelData.bills.length
                }
                ListView {
                    width: 200
                    height: 100
                    model: walletApp.wallets
                    delegate: Rectangle {
                        width: 200
                        height: 25
                        Text {
                            text: "Owner: " + model.modelData.currency
                        }
                    }
                }
            }*/
        }
    }

    function updateTitle(title) {
        console.log(walletApp.wallets[0].bills[0].address, walletApp.wallets[0].bills[0].publicKey, walletApp.wallets[0].bills[0].amount, walletApp.wallets[0].bills[0].currency)
        console.log(walletApp.wallets[0].bills[1].address, walletApp.wallets[0].bills[1].publicKey, walletApp.wallets[0].bills[1].amount, walletApp.wallets[0].bills[1].currency)
        walletApp.title = title;
    }
}
