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
            if (hasDarkWallet === true) {
                var lBrightBalance = userAccount.getBalance(iShortname, false, true)
                var lDarkBalance = userAccount.getBalance(iShortname, true, true)
                return threshodlTools.stringAdd(lBrightBalance, lDarkBalance)
            } else {
                return userAccount.getBalance(iShortname, true, true)
            }
        }

        function getMarketValue(iShortname) {
            return userAccount.getMarketValue(iShortname)
        }

        function getConfirmationText (iShortname, walletType) {
            if (userAccount.isWalletConfirmed(iShortname, walletType) === true) {
                return "(Confirmed)"
            } else {
                var lValue
                if (walletType === "Dark") {
                    lValue = userAccount.getBalance(iShortname, true, false)
                } else {
                    lValue = userAccount.getBalance(iShortname, false, false)
                }
                return "(" + lValue + " " + iShortname + " Pending)"
            }
        }

        Rectangle {
            y: 0
            x: parent.width*0.05
            height: (index === 0 ? 0 : 0.5)
            color: "lightgray"
            width: parent.width * 0.9
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Image {
            id: iconImage
            source: Qt.resolvedUrl(imageName)
            height: 50
            width: 50

            x: 20
            y: 25
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
            text: ""
            font.pointSize: 12
            font.weight: Font.Thin

            x: longNameText.x
            y: longNameText.y + longNameText.height + 10

            Connections {
                target: userAccount
                onMarketValueChanged: {
                    marketValueText.text = "$" + threshodlTools.formatMarketValueString(getMarketValue(shortName))
                }
            }
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

                onWalletBalanceUpdateComplete: {
                    if (shortname == shortName ) {
                        console.log("Balances updated")
                        totalCryptoText.text = getTotalConfirmedCryptoValue(shortName) + " " + shortName

                        confirmedCrytoStatus.text = getConfirmationText(shortName, "Bright");
                        if (userAccount.isWalletConfirmed(shortName, "Bright")) {
                            confirmedCrytoStatus.color = "#116F00"
                        } else {
                            confirmedCrytoStatus.color = "red"
                        }
                    }
                }
            }
        }

        Text {
            id: currencyValueOfTotalCryptoLabel

            text: "$" + threshodlTools.formatMarketValueString(userAccount.getBalanceValue(shortName))
            font.pointSize: 12
            font.bold: true

            x: totalCryptoText.x
            y: totalCryptoText.y + totalCryptoText.height + 5

            Connections {
                target: userAccount
                onWalletBalanceUpdateComplete: {
                    if (shortname == shortName ) {
                        currencyValueOfTotalCryptoLabel.text = "$" + threshodlTools.formatMarketValueString(userAccount.getBalanceValue(shortName))
                    }
                }
            }
        }

        Text {
            id: confirmedCrytoStatus

            text: "Loading..." //getConfirmationText(shortName, "Both");
            color: {
                if (userAccount.isWalletConfirmed(shortName, "Bright")) {
                    confirmedCrytoStatus.color = "#116F00"
                } else {
                    confirmedCrytoStatus.color = "red"
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
            height: 200

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
                        parent.height = parent.height + 190
                        expandCellButtonImage.source = "images/assets/upNavArrowIcon.png"
                        expandedWalletArea.visible = true
                    } else {
                        cellExpanded = !cellExpanded
                        parent.height = parent.height - 190
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
