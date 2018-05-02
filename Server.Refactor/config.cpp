#include "config.h"

#include <QFile>
#include <QJsonDocument>

Config::Config()
{
    mDefaultKeyFilter
            << QStringLiteral("CommandLineArgumentCount")
            << QStringLiteral("CommandLineArguments")
            << QStringLiteral("CAPrivateKeyFilename");
}

Config::Config(const QString iFilename)
    : mFilename(iFilename)
{ load(); }

Config::Config(const Config &iOther)
{
    _copy(iOther);
}

Config::~Config()
{ save(); }

Config &Config::operator=(const Config &iOther)
{
    if( this != &iOther )
        _copy(iOther);
    return *this;
}

QString Config::filename() const
{
    QReadLocker l{&mLock};
    return mFilename;
}

void Config::setFilename(const QString iFilename)
{
    QWriteLocker l{&mLock};
    mFilename = iFilename;
}

bool Config::load()
{
    QWriteLocker l{&mLock};

    if( ! mFilename.isEmpty() ) {
        QFile   lFile{mFilename};

        if( lFile.open(QIODevice::ReadOnly) ) {
            QJsonParseError lError;
            QVariantMap lMap = QJsonDocument::fromJson(lFile.readAll(),&lError).toVariant().toMap();
            if( lError.error == QJsonParseError::NoError ) {
                mConfigValues = lMap;
                return true;
            }
        }
    }

    return false;
}

bool Config::save(const QStringList iKeyFilter) const
{
    QReadLocker l{&mLock};

    if( ! mFilename.isEmpty() ) {
        QFile   lFile{mFilename};

        QVariantMap     lMap = mConfigValues;

        QStringList     lFilter = mDefaultKeyFilter;
        if( ! iKeyFilter.isEmpty() )
            lFilter = iKeyFilter;

        for( QString lKeyFilter : lFilter )
        { lMap.remove(lKeyFilter); }

        QByteArray lData = QJsonDocument::fromVariant(lMap).toJson();

        if( ! lData.isEmpty() ) {
            if( lFile.open(QIODevice::WriteOnly | QIODevice::Truncate) ) {
                if( lData.size() <= lFile.write(lData) ) {
                    return true;
                }
            }
        }
    }

    return false;
}

void Config::setPointer(const QString iKey, void *iPointer)
{
    QWriteLocker l{&mLock};
    if( iPointer == nullptr )
        mPointers.remove(iKey);
    else
        mPointers[iKey] = iPointer;
}

void Config::setValue(const QString iKey, QVariant iValue)
{
    QWriteLocker l{&mLock};
    mConfigValues[iKey] = iValue;
}

QVariant Config::value(const QString iKey) const
{
    QReadLocker l{&mLock};
    if( mConfigValues.contains(iKey) )
        return mConfigValues[iKey];
    return QVariant();
}

QStringList Config::keys() const
{
    QReadLocker l{&mLock};
    QStringList lList = mConfigValues.keys();
    for( QString lP : mPointers.keys() ) {
        if( lList.contains(lP) ) continue;
        lList << lP;
    }
    return lList;
}

QVariantMap Config::values() const
{
    QReadLocker l{&mLock};
    return mConfigValues;
}

void Config::_copy(const Config &iOther)
{
    QReadLocker lS{&iOther.mLock};
    QWriteLocker lD{&mLock};

    mFilename       = iOther.mFilename;
    mConfigValues   = iOther.mConfigValues;
    mPointers       = iOther.mPointers;
}

void *Config::toPointer(const QString iKey) const
{
    QReadLocker l{&mLock};
    if( mPointers.contains(iKey) )
        return mPointers[iKey];
    return nullptr;
}

void Config::removeValue(const QString iKey)
{
    QWriteLocker l{&mLock};
    mConfigValues.remove(iKey);
}

bool Config::contains(const QString iKey) const
{
    QReadLocker l{&mLock};
    return mConfigValues.contains(iKey);
}
