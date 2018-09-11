import QtQuick 2.0

ListModel {
    ListElement {
        name: "Testnet Bitcoin"
        shortName: "tBTC"

        imageName: "images/assets/testnetBitcoinIcon.png"
        brightIconName: "images/assets/brightBitcoinIcon.png"
        darkIconName: "images/assets/darkBitcoinIcon.png"

        hasDarkWallet: true
        cellExpanded: false
    }
    ListElement {
        name: "Bitcoin"
        shortName: "BTC"

        imageName: "images/assets/bitcoinIcon.png"
        brightIconName: "" // "images/assets/brightBitcoinIcon.png"
        darkIconName: "" // "images/assets/darkBitcoinIcon.png"

        hasDarkWallet: false
        cellExpanded: false
    }
    ListElement {
        name: "Testnet Litecoin"
        shortName: "tLTC"

        imageName: "images/assets/testnetLitecoinIcon.png"

        brightIconName: "images/assets/brightBitcoinIcon.png"
        darkIconName: "images/assets/darkBitcoinIcon.png"

        hasDarkWallet: false
        cellExpanded: false
    }
    ListElement {
        name: "Litecoin"
        shortName: "LTC"

        imageName: "images/assets/litecoinIcon.png"
        brightIconName: ""
        darkIconName: ""

        hasDarkWallet: false
        cellExpanded: false
    }
    ListElement {
        name: "Dash"
        shortName: "DASH"

        imageName: "images/assets/dashIcon.png"
        brightIconName: ""
        darkIconName: ""

        hasDarkWallet: false
        cellExpanded: false
    }
    ListElement {
        name: "ZCash"
        shortName: "ZEC"

        imageName: "images/assets/zcashIcon.png"
        brightIconName: ""
        darkIconName: ""

        hasDarkWallet: false
        cellExpanded: false
    }
//    ListElement {
//        name: "Bitcoin Cash"
//        shortName: "BCH"

//        imageName: "images/assets/bitcoinCashIcon.png"
//        brightIconName: ""
//        darkIconName: ""

//        hasDarkWallet: false
//        cellExpanded: false
//    }
//    ListElement {
//        name: "Ethereum"
//        shortName: "ETH"

//        imageName: "images/assets/ethereumIcon.png"
//        brightIconName: ""
//        darkIconName: ""

//        hasDarkWallet: false
//        cellExpanded: false
//    }
}
