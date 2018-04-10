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
                                                                  "MIIE6DCCAtCgAwIBAgIBATANBgkqhkiG9w0BAQ0FADAUMRIwEAYDVQQDDAlUaHJl"
                                                                  "c2hvZGwwHhcNMTgwNDEwMDAzMTE3WhcNMTkwNDEwMDAzMTE3WjAUMRIwEAYDVQQD"
                                                                  "DAlUaHJlc2hvZGwwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIKAoICAQCweqMb"
                                                                  "nrYwAjtCLEYN866DJ+wD2ASJyWbE7UnQenkgzBtYK78zrHTErJ/dYHPV+joJyvrW"
                                                                  "77xtfF8iEZSYw/o/eohKss26YtFKFJqZmYE+qeUyKVdCmNF7N8KnfWaLEn83GtOn"
                                                                  "3L0zJHjLFWvtZ7/vj+NtsaBGyKIww4bf4wIFg69ARZX0BvL4A+XyRYBkZUCmJG1k"
                                                                  "pkWetFXQHPcDwgIRVsoU1LA6I+d7wSPXWwa3q1lBiypAhpW6lR5osAvRroKfvX/P"
                                                                  "MKSVwIwZuO+2W4BuMa6gGD0pxM9wVBhULnVcNJC/Oj/cVEiUde+0ps+Ut0GMk1ZL"
                                                                  "oDZOrvh6AcrDZwDWhlKyeC5b8UNtpa9qXqiXWB55sHZTOBwTKRQJY1L1atwMSuRo"
                                                                  "JeYHsKo4RRSK+u2UbQmtbmI2SUE36jKORUCI2UxOuvVw0NMl+oVIpw61lIr356RI"
                                                                  "sBs+BNU2bAWlCFUjqF7/fsTsiNJS/gJF7N2HB3SF3laTjvISctlm+R1/zbVtFGas"
                                                                  "3NWtfKsX/bg5pasFiHJ0LA0mToRL/DjLP/l+Q+8dMc83sHCsDa2ZdkMfYneBiYuV"
                                                                  "Xb9JPuIMkNTn0Qq/9fUtG22Lo6merK56iNUieMugSB6ZAXpVOdC3tAbbeAPcsmub"
                                                                  "Z/6Q1tFymen2qLlFypicS+O6mKJsr/plMEb6bQIDAQABo0UwQzASBgNVHRMBAf8E"
                                                                  "CDAGAQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQU/PNnf2Hf0wn9qR9h"
                                                                  "UyjimWw2IIYwDQYJKoZIhvcNAQENBQADggIBABV1L274blg8K3SQhUNf08cLG67d"
                                                                  "92eX+8M6LK7ZK0Ctnw7PKaUCzVwjdSrL5eQO+pd1REKqN8GVefnIAY5B4QO/t7Gg"
                                                                  "enERu5DEWDqO0W+DkcqKWWDjp3YIR2svOG6qmqM3K7pDHfgsjnyWd4KpKCo+CVTf"
                                                                  "BDjsTjnsQHI2QiBe720/x3F42r/njh3wpv7zHgDYVbON+wyebSLyv9pfXLuMwI/P"
                                                                  "Pn3KexzNwXE9dMH3faFmt7M6d3dan8tzoNdg+Npx1xzshGppe/E2NHjb3xzCnZhE"
                                                                  "r4mCc2BBbh4uve/zeJZlv+HRiZOTBZ8VPnazOINYB4z1bK7VrYWtsn+bJXNNxFpx"
                                                                  "ikz0JxBnEtwL9LB6oRoK4oXX1ULMQ3qt4VXWB8KbhFyQPrLJRQ0kuWGNM5A9ZniP"
                                                                  "iBpmRki6t9g18Fvdc25WsnF9UvmhMot6XPXvDsXQSoeviLTvHzQKlzcaGH5dgtzu"
                                                                  "HZp5Hx7sRUwls3GTL6ZYEE8a/7t7FjRPbt5z+ZUrzK4wCS4c+UyJGY1uGPPHXYVR"
                                                                  "/FZrhyPPA4VUgoYhJg2EhvmcaCwyX1uhDtljuzWm3GsxYZW+IrTiUbjHI7+l3BHS"
                                                                  "T9TqFbmr0d2PczPd127AxfyRq8n9yD6iajI+cGSJnTS5t3vyPGrwFwxRyeNe+uwM"
                                                                  "8wha8o3yyXs+hDuT"
                                                                  "-----END CERTIFICATE-----"); }

#endif // GLOBALSANDSTYLE_H
