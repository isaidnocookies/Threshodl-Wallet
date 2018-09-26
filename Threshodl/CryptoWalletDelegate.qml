import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Component {
    id: cryptoRowDelegate

    Item {
        width: parent.width * cellWidthPercent
        height: 100

        Rectangle {
            id: walletCellBackground
            width: parent.width
            height: 100
            color: "white"

            radius: 25
            anchors.top: parent.top
            z: 1
        }

        function getTotalConfirmedCryptoValue (iShortname) {
            if (hasDarkWallet === true) {
                var lBrightBalance = userAccount.getBalance(iShortname, true)
                var lDarkBalance = userAccount.getBalance("d" + iShortname, true)
                return threshodlTools.stringAdd(lBrightBalance, lDarkBalance)
            } else {
                return userAccount.getBalance(iShortname, true)
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
                    lValue = userAccount.getBalance(iShortname, false)
                } else {
                    lValue = userAccount.getBalance(iShortname, false)
                }
                return "(" + lValue + " " + iShortname + " Pending)"
            }
        }

        Image {
            id: iconImage
            source: Qt.resolvedUrl(imageName)
            height: 50
            width: 50

            x: 20
            y: 25
            z: 10
        }

        Text {
            id: longNameText
            text: name
            font.bold: true
            font.pointSize: 15

            y: iconImage.y + 8
            x: iconImage.x + iconImage.width + 10
            z:10
        }

        Text {
            id: marketValueText
            text: ""
            font.pointSize: 12
            font.weight: Font.Thin

            x: longNameText.x
            y: longNameText.y + longNameText.height + 10
            z: 10

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
            x: parent.width - 20 - width //parent.width / 2 + parent.width * 0.1
            z: 10

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

            z:10

            font.pointSize: 12
            font.bold: true

            x: parent.width - 20 - width //totalCryptoText.x
            y: totalCryptoText.y + totalCryptoText.height + 5

            Component.onCompleted: formatValueText()

            Connections {
                target: userAccount
                onWalletBalanceUpdateComplete: {
                    if (shortname == shortName ) {
                        currencyValueOfTotalCryptoLabel.formatValueText()
                    }
                }

                onMarketValueChanged: {
                    currencyValueOfTotalCryptoLabel.formatValueText()
                }
            }

            function formatValueText() {

                var darkBalance = "0.00";
                var brightBalance = userAccount.getBalanceValue(shortName);

                if (hasDarkWallet) {
                    darkBalance = userAccount.getBalanceValue("d" + shortName);
                }

                var text = "$" + threshodlTools.formatMarketValueString(threshodlTools.stringAdd(darkBalance, brightBalance));
                if (text !== "$0.00") {
                    currencyValueOfTotalCryptoLabel.text = text
                } else {
                    currencyValueOfTotalCryptoLabel.text = "$0.00"
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
            x: parent.width - 20 - width //totalCryptoText.x
            y: currencyValueOfTotalCryptoLabel.y + currencyValueOfTotalCryptoLabel.height + 5

            font.weight: Font.Thin
            font.pointSize: 11

            z:10
        }

        Button {
            id:expandCellButton
            visible: hasDarkWallet

            height: 20
            width: 20
            z: 10

            background: Rectangle {
                color: "transparent"
                width: parent.height
                height: parent.width
                anchors.centerIn: parent
            }

            x: (parent.width / 2) - (width / 2)
            y: marketValueText.y + marketValueText.height + 5

            Image {
                id: expandCellButtonImage
                source: "images/assets/dotdotdotIcon.png"
                fillMode: Image.PreserveAspectFit
                width: parent.width
            }
        }

        Item {
            id: expandedWalletArea
            visible: false

            x: 0
            width: parent.width

            Rectangle {
                x: 0
                width: parent.width
                y: iconImage.y + iconImage.height/2
                height: (darkWalletCell.y + darkWalletCell.height) - y
                color: "white"
                z:1
                radius: 20
            }

            Rectangle {
                id: topExpandedShadowEffect
                y: walletCellBackground.y + walletCellBackground.height
                height: 8
                width: parent.width
                z:9
                opacity: 0.4

                gradient: Gradient {
                    GradientStop {
                        position: 0
                        color: Qt.rgba(0, 0, 0, 0.5)
                    }
                    GradientStop {
                        position: 1
                        color: Qt.rgba(0, 0, 0, 0)
                    }
                }
            }

            ExpandedCellComponent
            {
                id: brightWalletCell
                height: 80
                y: iconImage.y + iconImage.height + 30
                z:1
                iconPath: brightIconName
                name: shortName
                expandedShortName: shortName

                color: "transparent"
            }

            Rectangle {
                y: brightWalletCell.y + brightWalletCell.height
                height: 1
                z: 1
                color: "lightgray"
                width: parent.width * 0.8
                x: parent.width * 0.15
            }

            ExpandedCellComponent
            {
                id: darkWalletCell
                height: 85
                y: brightWalletCell.y + brightWalletCell.height + 10
                z:1
                iconPath: darkIconName
                name: "Dark " + shortName
                expandedShortName: "d" + shortName

                color: "transparent"
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
                        parent.height = parent.height + 181
                        expandedWalletArea.visible = true
                    } else {
                        cellExpanded = !cellExpanded
                        parent.height = parent.height - 181
                        expandedWalletArea.visible = false
                    }
                } else {
                    console.log("Clicked non-dark wallet")
                    ourStackView.push(brightWallet)
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
