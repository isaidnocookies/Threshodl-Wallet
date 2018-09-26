import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: expandedWalletComponent

    property alias iconPath: littleIconImage.source
    property alias name: nameText.text
    property string expandedShortName

    width: parent.width

    function getBalances (cryptoType, isConfirmed) {
        return userAccount.getBalance(cryptoType, isConfirmed)
    }

    function getCurrencyValue(cryptoType, toCurrency) {
        return userAccount.getBalanceValue(cryptoType)
    }

    function isDark() {
        if (name.search("Dark") != -1) {
            return true
        }
        return false
    }

    function getBaseShortname(iShortname) {
        var editedShortname = iShortname;

        if (editedShortname.charAt(0) === "d") {
            editedShortname = editedShortname.substring(1);
        }

        return editedShortname
    }

    Connections {
        target: userAccount

        onMarketValueChanged: {
            if (shortname === expandedShortName || shortname === getBaseShortname(expandedShortName)) {
                currencyValueOfTotalCryptoLabelExpanded.text = getCurrencySymbol("USD") + threshodlTools.formatMarketValueString(userAccount.getBalanceValue(expandedShortName));
            }
        }
    }

    Item {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            color: "transparent"
        }

        Image {
            id: littleIconImage
            source: Qt.resolvedUrl(iconPath)
            height: 25
            width: 25

            x: 30
            y: 25
        }

        Text {
            id: nameText
            text: name
            font.weight: Font.Thin
            font.pointSize: 16

            y: littleIconImage.y + (littleIconImage.height/2) - (height/2)
            x: littleIconImage.x + littleIconImage.width + 10
        }

        Text {
            id: totalCryptoTextExpanded
            text: {
                if (isDark) {
                    return getBalances(expandedShortName, false) + " " + expandedShortName
                }
                return getBalances(expandedShortName, true) + " " + expandedShortName
            }
            font.pointSize: 16
            font.weight: Font.Thin

            y: nameText.y - 12
            x: parent.width - width - 20 //parent.width / 2 + parent.width * 0.1
        }

        Text {
            id: currencyValueOfTotalCryptoLabelExpanded

            text: getCurrencySymbol("USD") + threshodlTools.formatMarketValueString(getCurrencyValue(expandedShortName, "USD"))
            font.pointSize: 12
            font.bold: true

            x: parent.width - width - 20 //totalCryptoTextExpanded.x
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
            x: parent.width - width - 20 //totalCryptoTextExpanded.x
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
                walletShortName: expandedShortName
                walletIconPath: iconPath
            }
        }

        Component {
            id: expandedDarkWallet

            DarkWalletView {
                walletShortName: expandedShortName
                walletIconPath: iconPath
            }
        }
    }
}
