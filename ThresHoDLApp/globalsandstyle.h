#ifndef GLOBALSANDSTYLE_H
#define GLOBALSANDSTYLE_H

#include <QString>
#include <QStringLiteral>

//#define TEST_SERVER_IP_ADDRESS "wss://10.10.0.78:4431"
//#define TEST_SERVER_IP_ADDRESS "wss://10.10.0.35:4431"

#define TEST_SERVER_IP_ADDRESS "wss://bluewallet.threebx.com:4431"

inline QString theOrganization() {              return QString("ThreeB"); }
inline QString theApplicationName() {           return QString("Threshodl"); }
inline QString theDomain() {                    return QString("threshodl.com"); }

inline QString usernameSetting() {              return QString("usernameSetting"); }
inline QString publicKeySetting() {             return QString("publicKeySetting"); }
inline QString privateKeySetting() {            return QString("privateKeySetting"); }
inline QString brightWalletsSetting() {         return QString("brightWalletsSetting"); }
inline QString darkWalletsSetting() {           return QString("darkWalletsSetting"); }
inline QString darkWalletPendingSetting() {     return QString("darkWalletPendingSetting"); }

inline QString emailSetting() {                 return QString("emailSetting"); }
inline QString addressSetting() {               return QString("addressSetting"); }

inline QString notificationsSetting() {         return QString("notificationsSettings"); }

inline QString myDateFormat() {                 return QString("d.M.yy"); }

inline QString jsonPrivateKey() {               return QString("jsonPrivateKey"); }
inline QString jsonPublicKey() {                return QString("jsonPublicKey"); }
inline QString jsonAmount() {                   return QString("jsonAmount"); }
inline QString jsonEmailAddress() {             return QString("jsonEmail"); }
inline QString jsonThreshodlAddress() {         return QString("jsonThreshodlAddress"); }

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
                                                                    "font: 12px;"
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
                                                                    "font: 12px;"
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

#endif // GLOBALSANDSTYLE_H
