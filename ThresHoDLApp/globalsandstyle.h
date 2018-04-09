#ifndef GLOBALSANDSTYLE_H
#define GLOBALSANDSTYLE_H

#include <QString>

inline QString theOrganization() {              return QString("ThreeB"); }
inline QString theApplicationName() {           return QString("Threshodl"); }
inline QString theDomain() {                    return QString("threshodl.com"); }

inline QString usernameSetting() {              return QString("usernameSetting"); }
inline QString publicKeySetting() {             return QString("publicKeySetting"); }
inline QString privateKeySetting() {            return QString("privateKeySetting"); }

inline QString emailSetting() {                 return QString("emailSetting"); }
inline QString addressSetting() {               return QString("addressSetting"); }

//03a9f4
//old light blue - 4c586F
//old dark blue - 404859

inline QString lightBackgroundStyleSheet() {    return QString("QPushButton {"
                                                                    "background-color: #03a9f4;"
                                                                    "border-style: outset;"
                                                                    "border-width: 2px;"
                                                                    "border-radius: 10px;"
                                                                    "border-color: #0399dd;"
                                                                    "font: bold 14px;"
                                                                    "color: white;"
                                                                    "padding: 10px;"
                                                               "}"
                                                               "QPushButton:pressed {"
                                                                    "background-color: #0399dd;"
                                                               "}"
                                                               "QLineEdit {"
                                                                    "border: 2px solid #a2aab0;"
                                                                    "border-radius: 10px;"
                                                                    "padding: 5px 8px;"
                                                                "}"
//                                                                "QCheckBox:indicator {"
//                                                                    "border-color: #a2aab0;"
//                                                                    "color: black;"
//                                                                "}"
//                                                                "QCheckBox {"
//                                                                   "color: black;"
//                                                                "}"
                                                                ); }

inline QString darkBackgroundStyleSheet() {    return QString("QPushButton {"
                                                                    "background-color: #3e3e3b;"
                                                                    "border-style: outset;"
                                                                    "border-width: 2px;"
                                                                    "border-radius: 10px;"
                                                                    "border-color: #2d2d2c;"
                                                                    "font: bold 14px;"
                                                                    "color: white;"
                                                                    "padding: 10px;"
                                                               "}"
                                                               "QPushButton:pressed {"
                                                                    "background-color: #2d2d2c;"
                                                               "}"
                                                               "QLineEdit {"
                                                                    "border: 2px solid #3e3e3b;"
                                                                    "border-radius: 10px;"
                                                                    "padding: 5px 8px;"
                                                                    "color: white;"
                                                               "}"
//                                                               "QCheckBox:indicator {"
//                                                                    "border-color: #3e3e3b;"
//                                                                    "background-color: white;"
//                                                                    "color: black;"
//                                                               "}"
//                                                               "QCheckBox {"
//                                                                   "color: white;"
//                                                               "}"
                                                               ); }

inline QByteArray serverCert() {                return QByteArray("-----BEGIN CERTIFICATE-----"
                                                          "MIIE+zCCAuOgAwIBAgIBATANBgkqhkiG9w0BAQ0FADAUMRIwEAYDVQQDDAlUaHJl"
                                                          "c2hvZGwwHhcNMTgwNDA1MjMyOTE3WhcNMTkwNDA1MjMyOTE3WjAUMRIwEAYDVQQD"
                                                          "DAlUaHJlc2hvZGwwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCaLyG+"
                                                          "V9h8Jbgbw1tcotnMLW8HZZ4CXuZmMCMQSEHjDJRkrGrp990VpCuvvsxvTUyPk+aH"
                                                          "dWwjju/ZrXGFCAEo5nhaRSdgoSCEaX0jQJXCXOsigeuOUX2ms0wYpd9Wprgs4jlO"
                                                          "HEt9EctmQFc2vrqu522N1Mam7YsqsYxD+++RWakeY1l2DuBZXBu+PcEa4Q+q9f2L"
                                                          "GoadUpwQBnAaEdvhqxX17vohDXjtGD7aBmdvvu5fXDTL0hfVzHwdgeQUzfMExcyL"
                                                          "SxlgMrxh4l1iNJBI9QoiVW4GBI0XR+lTqIFKCAdkUbTsk3NVcwRo5JrVWDrUIJBZ"
                                                          "QOg0PdR7LhpM0qddurRQDMm2X0ZCu6okShQ/BGzztyy9HDq6dXdP0r5ZuAKFgnZD"
                                                          "nE33ljsuiDUa37D0DdMEZleagtheeTnco0/V927GCFhbHAsiHK1uL88YE1cswkIa"
                                                          "HZZOOny8Yq80SB+T2na/4u4XpZW65MCX8QwLABJ4ijiJeXyXpPrERZD2OIgGAwSr"
                                                          "4G+LzhCKhL2ZNRzvwgHt3tJlFWDVc7rmW7q9CWOqrhWec8I18/Bkd3nMiFGJUkVc"
                                                          "4zByRIGuQByFsrtJPVyff3J36yj/3VapagUckF74Aa1/GvxL4jXdinpXRTBdcVCw"
                                                          "XQRgDxTTNMg+A5qsjHCdatDgtumpRXZPG1j6bQIDAQABo1gwVjASBgNVHRMBAf8E"
                                                          "CDAGAQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQU/p8gfUNZm9mFeHd0"
                                                          "ZghV90UtoVwwEQYJYIZIAYb4QgEBBAQDAgIEMA0GCSqGSIb3DQEBDQUAA4ICAQCW"
                                                          "yO2GelRjYtTsopLdIswuG6ln4+n/TN0SzFsaayJBqSEqbz8lDehXyxFw7UiH4LVk"
                                                          "+qrhNLsTiwSAO0sJ+4S+m/Klu5RsskZomJF5c1PobCze2OxTQt53cGJD2praa/3U"
                                                          "bgF7rYyuFLfNFRJmWU1UxBddUPWPDSk74oP+5XvMHALh62eoMUq7rM+zg5Xn7ENb"
                                                          "iuhsZEUItPSbjX6Gi2+BwgoyWnXuHfHvvImmLNdm+4F9odxmTJBVYJuRFz+Nwvrf"
                                                          "M1fkkHqkVmAyXZRPDgtacXM+ij8He+UKDS9ywT5qAGVEpPEQEmZAHeDlr198pu6y"
                                                          "yjh58Dn9SRIK6GqesTIjU/xerLWcVlkZ6l1n1I3wWwxpRzMbV/AS9shMuOOCagBJ"
                                                          "GJA8lL+eqSyvJbIZMrcbxuINnsKXtx5SwlErt1N0VpKUmPd1nzRLS2p03NM0N0UB"
                                                          "SUhXe50iKZtNKsMqboNe71/d0u5EBAI1wjTqUKlZX1GaiJMeFgqJU8IO7FDTBmgq"
                                                          "kg7h1iO7UxWb8GAUUtvJ3FMN/k9YVvW4arT1LTrmLa5XtiYcnnafEIxqlQqbWBqt"
                                                          "8sT1OqM5mLIPb3Ss6WM4lMP0pzE9gGt9TlkueoH/lz4LEtES4fF/pUsxSxmBaCAa"
                                                          "uoUYCfKua5s1vl7DgwZPsnxNLsAvhrkSZYxb3D+nVw=="
                                                          "-----END CERTIFICATE-----"); }

#endif // GLOBALSANDSTYLE_H
