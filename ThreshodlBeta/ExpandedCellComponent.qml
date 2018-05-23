import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: expandedWalletComponent

    property alias iconPath: littleIconImage.source
    property alias name: nameText.text

    width: parent.width

    // cryptoType -> BTC, LTC, ETH
    // mode -> Bright or Dark
    function getBalances (cryptoType, isBright, isConfirmed) {
        var lMode
        if (isBright) {
            lMode = "Bright"
        } else {
            lMode = "Dark"
        }

        if (cryptoType === "BTC") {
            return userAccount.getBitcoinBalance(lMode, isConfirmed)
        } else if (cryptoType === "ETH") {
            if (isConfirmed) {
                return userAccount.ethereumConfirmedBalance
            }
            return userAccount.ethereumUnonfirmedBalance
        } else if (cryptoType === "LTC") {
            if (isConfirmed) {
                return userAccount.litecoinConfirmedBalance
            }
            return userAccount.litecoinUnconfirmedBalance
        } else {
            // Error
            console.log("Error... Unrecognized crypto type")
        }
    }

    function getCurrencyValue(cryptoType, currencyType, cryptoAmount) {
        return "0.00"
    }

    function isDark() {
        if (name.search("Dark") != -1) {
            return true
        }
        return false
    }

    Item {
        width: parent.width
        height: 80

        Image {
            id: littleIconImage
            source: Qt.resolvedUrl(iconPath)
            height: 20
            width: 20

            x: 60
            y: parent.height/2 - iconImage.height/2
        }

        Text {
            id: nameText
            text: name
            font.weight: Font.Thin
            font.pointSize: 16

            y: littleIconImage.y + littleIconImage.height/2 - (height/2) - 3
            x: littleIconImage.x + littleIconImage.width + 10
        }

        Text {
            id: totalCryptoTextExpanded
            text: {
                if (isDark()) {
                    return getBalances(shortName, false, true) + " " + shortName
                } else {
                    return getBalances(shortName, true, true) + " " + shortName
                }
            }
            font.pointSize: 16
            font.weight: Font.Thin

            y: nameText.y - 10
            x: parent.width / 2 + parent.width * 0.1
        }

        Text {
            id: currencyValueOfTotalCryptoLabelExpanded

            text: getCurrencySymbol("USD") + getCurrencyValue(shortName, "USD", function() {
                if (isDark) {
                    return getBalances(shortName, false, true) + " -- " + shortName
                } else {
                    return getBalances(shortName, true, true) + " " + shortName
                }
            }) + " " + "USD"
            font.pointSize: 12
            font.bold: true

            x: totalCryptoTextExpanded.x
            y: totalCryptoTextExpanded.y + totalCryptoTextExpanded.height + 5
        }

        Text {
            id: confirmedCrytoStatusExpanded

            text: getConfirmationText(shortName, "Both");
            color: {
                if (confirmedCrytoStatus.text === "(Confirmed)") {
                    return "#116F00"
                } else {
                    return "red"
                }
            }
            x: totalCryptoTextExpanded.x
            y: currencyValueOfTotalCryptoLabelExpanded.y + currencyValueOfTotalCryptoLabelExpanded.height + 5

            font.weight: Font.Thin
            font.pointSize: 11
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: {
                if (isDark() === false) {
                    ourStackView.push(expandedBrightWallet)
                } else {
                    ourStackView.push(expandedDarkWallet)
                }
            }
        }

        Component {
            id: expandedBrightWallet

            BrightWalletView {
                walletShortName: shortName
                walletIconPath: iconPath
            }
        }

        Component {
            id: expandedDarkWallet

            DarkWalletView {
                walletShortName: shortName
                walletIconPath: iconPath
            }
        }
    }
}
