import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0

Rectangle {
    id: expandedWalletComponent

    property alias iconPath: littleIconImage.source
    property alias name: nameText.text

    width: parent.width

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
            text: "0.134" + " " + shortName
            font.pointSize: 16
            font.weight: Font.Thin

            y: nameText.y - 10
            x: parent.width / 2 + parent.width * 0.1
        }

        Text {
            id: currencyValueOfTotalCryptoLabelExpanded

            text: "$100.00 USD"
            font.pointSize: 12
            font.bold: true

            x: totalCryptoTextExpanded.x
            y: totalCryptoTextExpanded.y + totalCryptoTextExpanded.height + 5
        }

        Text {
            id: confirmedCrytoStatusExpanded

            text: "(Confirmed)"
            color: "#116F00"
            x: totalCryptoTextExpanded.x
            y: currencyValueOfTotalCryptoLabelExpanded.y + currencyValueOfTotalCryptoLabelExpanded.height + 5

            font.weight: Font.Thin
            font.pointSize: 11
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: {
                // go to wallet
                console.log("Clicked dark compatible sub wallet")
            }
        }
    }
}
