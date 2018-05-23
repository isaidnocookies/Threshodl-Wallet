import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Component {
    id: cryptoRowDelegate

    Item {
        width: parent.width
        height: 100

        function getTotalConfirmedCryptoValue (iShortname) {
            if (iShortname === "BTC") {
                return userAccount.bitcoinConfirmedBalance
            } else if (iShortname === "ETH") {
                return userAccount.ethereumConfirmedBalance
            } else if (iShortname === "LTC") {
                return userAccount.litecoinConfirmedBalance
            }
        }

        function getConfirmationText (iShortname, walletType) {
            if (userAccount.isWalletConfirmed(iShortname, walletType) === true) {
                return "(Confirmed)"
            } else {
                return "(" + (function() {
                    if (iShortname === "BTC") {
                        if (walletType === "Dark") {
                            userAccount.getBitcoinBalance("Dark", false)
                        } else if (walletType === "Bright") {
                            return userAccount.getBitcoinBalance("Bright", false)
                        } else {
                            return userAccount.bitcoinUnconfirmedBalance
                        }
                    } else if (iShortname === "ETH") {
                        return userAccount.ethereumUnconfirmedBalance
                    } else if (iShortname === "LTC") {
                        return userAccount.litecoinUnconfirmedBalance
                    }
                }) + " " + iShortname + ")"
            }
        }

        Image {
            id: iconImage
            source: Qt.resolvedUrl(imageName)
            height: 50
            width: 50

            x: 20
            y: 10
        }

        Text {
            id: longNameText
            text: name
            font.bold: true
            font.pointSize: 15

            y: iconImage.y + 10
            x: iconImage.x + iconImage.width + 10
        }

        Text {
            id: marketValueText
            text: "$10,000 USD"
            font.pointSize: 12
            font.weight: Font.Thin

            x: longNameText.x
            y: longNameText.y + longNameText.height + 10
        }

        Text {
            id: totalCryptoText
            text: getTotalConfirmedCryptoValue(shortName) + " " + shortName
            font.pointSize: 16
            font.weight: Font.Thin

            y: longNameText.y - 8
            x: parent.width / 2 + parent.width * 0.1

            Connections {
                target: userAccount
                onCryptoConfirmedBalanceChanged: {
                    totalCryptoText.text = getTotalConfirmedCryptoValue(shortName) + " " + shortName
                }
            }
        }

        Text {
            id: currencyValueOfTotalCryptoLabel

            text: "$0.00 USD"
            font.pointSize: 12
            font.bold: true

            x: totalCryptoText.x
            y: totalCryptoText.y + totalCryptoText.height + 5
        }

        Text {
            id: confirmedCrytoStatus

            text: getConfirmationText(shortName, "Both");
            color: {
                if (confirmedCrytoStatus.text === "(Confirmed)") {
                    return "#116F00"
                } else {
                    return "red"
                }
            }
            x: totalCryptoText.x
            y: currencyValueOfTotalCryptoLabel.y + currencyValueOfTotalCryptoLabel.height + 5

            font.weight: Font.Thin
            font.pointSize: 11
        }

        Button {
            id:expandCellButton
            visible: hasDarkWallet

            height: 8
            width: 15

            background: Rectangle {
                color: "white"
                width: parent.height
                height: parent.width
                anchors.centerIn: parent
            }

            x: parent.width - 30
            y: iconImage.y + iconImage.height / 2 - height/2

            Image {
                id: expandCellButtonImage
                source: "images/assets/downNavArrowIcon.png"
                fillMode: Image.PreserveAspectFit
                width: parent.width
            }
        }

        Item {
            id: expandedWalletArea
            visible: false

            x: 0

            width: parent.width
            height: 150

            ExpandedCellComponent
            {
                id: brightWalletCell
                height: 80
                y: iconImage.y + iconImage.height + 30
                iconPath: brightIconName
                name: shortName
            }

            ExpandedCellComponent
            {
                id: darkWalletCell
                height: 80
                y: brightWalletCell.y + brightWalletCell.height + 10
                iconPath: darkIconName
                name: "Dark " + shortName
            }

        }

        MouseArea {
            id: mouseArea
            anchors.top: parent.top
            height: 100
            width: parent.width

            onClicked: {
                if (hasDarkWallet) {
                    if (!cellExpanded) {
                        cellExpanded = !cellExpanded
                        parent.height = parent.height + 160
                        expandCellButtonImage.source = "images/assets/upNavArrowIcon.png"
                        expandedWalletArea.visible = true
                    } else {
                        cellExpanded = !cellExpanded
                        parent.height = parent.height - 160
                        expandedWalletArea.visible = false
                        expandCellButtonImage.source = "images/assets/downNavArrowIcon.png"

                    }
                } else {
                    //go to wallet
                    console.log("Clicked non-dark wallet")
                    ourStackView.push(brightWallet)
//                    ourStackView.push(BrightWalletView{walletShortName: shortName})
                }
            }
        }

        Component {
            id: brightWallet

            BrightWalletView {
                walletShortName: shortName
                walletIconPath: imageName
            }
        }

    }
}
