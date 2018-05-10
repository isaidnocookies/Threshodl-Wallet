#ifndef FEEESTIMATORINTERFACE_H
#define FEEESTIMATORINTERFACE_H

#include "qstringmath.h"

#include <QString>
#include <QMap>
#include <QReadWriteLock>
#include <QObject>

typedef enum CommonFeeType_e
{
    TestNetBaseFee      = 0,
    TestNetInputFee,
    TestNetOutputFee,
    MainNetBaseFee,
    MainNetInputFee,
    MainNetOutputFee
} CommonFeeType;

class FeeEstimatorInterface : public QObject
{
    Q_OBJECT
protected:
    typedef QMap< QString, QStringMath >    FeeEntry;

    QReadWriteLock              mFeesLock;
    QMap< QString, FeeEntry >   mFees;

public:
    QString commonTypeToString(CommonFeeType iType)
    {
        switch(iType)
        {
        case TestNetBaseFee:        return QStringLiteral("TestNetBaseFee");
        case TestNetInputFee:       return QStringLiteral("TestNetInputFee");
        case TestNetOutputFee:      return QStringLiteral("TestNetOutputFee");
        case MainNetBaseFee:        return QStringLiteral("MainNetBaseFee");
        case MainNetInputFee:       return QStringLiteral("MainNetInputFee");
        case MainNetOutputFee:      return QStringLiteral("MainNetOutputFee");
        default:                    return QStringLiteral("*");
        }
    }

    FeeEstimatorInterface( QObject * iParent = nullptr ) : QObject(iParent)
    {
        qRegisterMetaType<CommonFeeType>("CommonFeeType");
    }

    virtual ~FeeEstimatorInterface()
    { }

    virtual QStringMath getFee(const QString iCryptoCurrency, const QString iType)
    {
        QReadLocker l{&mFeesLock};
        return mFees[iCryptoCurrency][iType];
    }

    virtual QStringMath getFee(const QString iCryptoCurrency, CommonFeeType iType)                  { return getFee(iCryptoCurrency,commonTypeToString(iType)); }
    virtual QStringMath getFee(const QString iCryptoCurrency)                                       { return getFee(iCryptoCurrency,QStringLiteral("*")); }
    virtual QMap< QString, QStringMath > getFees( const QString iCryptoCurrency, const QStringList iTypes )
    {
        QMap< QString, QStringMath > lFees;
        QReadLocker l{&mFeesLock};
        for( QString lType : iTypes )
        { lFees[lType] = mFees[iCryptoCurrency][lType]; }
        return lFees;
    }

signals:
    void recordFee(const QString iCryptoCurrency, const QString iType, const QString iFee);

public slots:
    virtual void setFee(const QString iCryptoCurrency, const QString iType, const QString iFee)
    {
        mFeesLock.lockForWrite();
        mFees[iCryptoCurrency][iType] = QStringMath{iFee};
        mFeesLock.unlock();
    }

    virtual void setFee(const QString iCryptoCurrency, CommonFeeType iType, const QString iFee)     { setFee(iCryptoCurrency,commonTypeToString(iType),iFee); }
    virtual void setFee(const QString iCryptoCurrency, const QString iFee)                          { setFee(iCryptoCurrency,QStringLiteral("*"),iFee); }
    virtual void setFees(const QString iCryptoCurrency, const QMap< QString, QString > iFees )
    {
        mFeesLock.lockForWrite();
        for( QString lType : iFees.keys() )
        { mFees[iCryptoCurrency][lType] = QStringMath{iFees[lType]}; }
        mFeesLock.unlock();
    }
};

#endif // FEEESTIMATORINTERFACE_H
