#include "walletdatacore.h"

bool WalletDataCoreDeserialize(const QByteArray iData, WalletDataCore &oWalletDataCore)
{
    QJsonParseError lError;
    oWalletDataCore = QJsonDocument::fromJson(iData,&lError).toVariant().toMap();
    return lError == QJsonParseError::NoError;
}
