#ifndef WALLETGRINDER_H
#define WALLETGRINDER_H

#include "qstringmath.h"

#include <QReadWriteLock>
#include <QString>
#include <QMap>
#include <QStringList>

class WalletGrinder
{
protected:
    QReadWriteLock              mFeesLock;
    QMap< QString, QString >    mBaseFees;
    QMap< QString, QString >    mPerWalletFees;

public:
    WalletGrinder() { }

    virtual void setCurrentNetworkFeeBase(const QString iFee, const QString iCryptoCurrency = QStringLiteral("BTC"))
    { QWriteLocker l{&mFeesLock}; mBaseFees[iCryptoCurrency] = iFee; }

    virtual void setCurrentNetworkFeePerWallet(const QString iFee, const QString iCryptoCurrency = QStringLiteral("BTC"))
    { QWriteLocker l{&mFeesLock}; mPerWalletFees[iCryptoCurrency] = iFee; }

    virtual void setCurrentNetworkFees(const QString iBaseFee, const QString iPerWalletFee, const QString iCryptoCurrency = QStringLiteral("BTC"))
    { QWriteLocker l{&mFeesLock}; mBaseFees[iCryptoCurrency] = iBaseFee; mPerWalletFees[iCryptoCurrency] = iPerWalletFee; }

    virtual QString getNetworkFeeBase(const QString iCryptoCurrency = QStringLiteral("BTC"))
    { QReadLocker l{&mFeesLock}; if( mBaseFees.contains(iCryptoCurrency) ) return mBaseFees[iCryptoCurrency]; else return QStringLiteral("0.0"); }

    virtual QString getNetworkFeePerWallet(const QString iCryptoCurrency = QStringLiteral("BTC"))
    { QReadLocker l{&mFeesLock}; if( mPerWalletFees.contains(iCryptoCurrency) ) return mPerWalletFees[iCryptoCurrency]; else return QStringLiteral("0.0"); }

    virtual void getNetworkFees(QString &oBaseFee, QString &oPerWalletFee, const QString iCryptoCurrency = QStringLiteral("BTC"))
    {
        QReadLocker l{&mFeesLock};
        if( mBaseFees.contains(iCryptoCurrency) )
            oBaseFee = mBaseFees[iCryptoCurrency];
        else
            oBaseFee = QStringLiteral("0.0");

        if( mPerWalletFees.contains(iCryptoCurrency) )
            oPerWalletFee = mPerWalletFees[iCryptoCurrency];
        else
            oPerWalletFee = QStringLiteral("0.0");
    }

    virtual QString estimateNetworkFees(const QStringList iWalletValues, const QString iCryptoCurrency = QStringLiteral("BTC"))
    {
        double  lWalletCount    = iWalletValues.size();
        QString lBaseFee;
        QString lPerWalletFee;
        getNetworkFees(lBaseFee,lPerWalletFee,iCryptoCurrency);

        QStringMath lWalletFee{QString::number(lWalletCount * lPerWalletFee.toDouble())};

        return lWalletFee.add(lBaseFee).toString();
    }

    virtual QStringList grindValue(const QString iValue, const QString iCryptoCurrency = QStringLiteral("BTC"))                                     = 0;
};

#endif // WALLETGRINDER_H
