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
    function getBalances (cryptoType, isDark, isConfirmed) {
        return userAccount.getBalance(cryptoType, isDark, isConfirmed)
    }

    function getCurrencyValue(cryptoType, isDark, toCurrency) {
        return userAccount.getBalanceValue(cryptoType, isDark)
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

        Rectangle {
            height: 0.5
            width: parent.width * 0.8
            y: 0
            x: parent.width - width - (parent.width * 0.05)
            color: "lightgray"
        }

        Image {
            id: littleIconImage
            source: Qt.resolvedUrl(iconPath)
            height: 20
            width: 20

            x: 60
            y: 35
//            y: parent.height/2 - iconImage.height/2
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
                    return getBalances(shortName, true, true) + " " + shortName
                } else {
                    return getBalances(shortName, false, true) + " " + shortName
                }
            }
            font.pointSize: 16
            font.weight: Font.Thin

            y: nameText.y - 10
            x: parent.width / 2 + parent.width * 0.1
        }

        Text {
            id: currencyValueOfTotalCryptoLabelExpanded

            text: getCurrencySymbol("USD") + getCurrencyValue(shortName, isDark, "USD") + " " + "USD"
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
