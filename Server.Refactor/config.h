#ifndef CONFIG_H
#define CONFIG_H

#include <QReadWriteLock>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVariantMap>

class Config
{
public:
    Config();
    Config(const QString iFilename);
    Config(const Config &iOther);
    ~Config();

    Config & operator=(const Config & iOther);

    void                    setDisableSave(bool iOn = true);

    QString                 filename() const;
    void                    setFilename(const QString iFilename);

    bool                    load();
    bool                    save(const QStringList iKeyFilter = QStringList()) const;   // Returns true on disabled save

    void                    setPointer(const QString iKey, void * iPointer);            // Not savable/loadable
    void *                  toPointer(const QString iKey) const;                        // Not savable/loadable

    void                    setValue(const QString iKey, QVariant iValue);
    void                    setValue(const QString iKey, QByteArray iValue)             { setValue(iKey, QVariant{iValue.toBase64()}); }
    void                    removeValue(const QString iKey);

    bool                    contains(const QString iKey) const;

    QStringList             keys() const;
    QVariantMap             values() const;

    QVariant                value(const QString iKey) const;

    qreal                   toReal(const QString iKey, bool * oOk = nullptr) const      { return value(iKey).toReal(oOk); }
    qint8                   toInt8(const QString iKey, bool * oOk = nullptr) const      { return static_cast<qint8>(value(iKey).toInt(oOk)); }
    quint8                  toUInt8(const QString iKey, bool * oOk = nullptr) const     { return static_cast<quint8>(value(iKey).toUInt(oOk)); }
    qint16                  toInt16(const QString iKey, bool * oOk = nullptr) const     { return static_cast<qint16>(value(iKey).toInt(oOk)); }
    quint16                 toUInt16(const QString iKey, bool * oOk = nullptr) const    { return static_cast<quint16>(value(iKey).toUInt(oOk)); }
    qint32                  toInt32(const QString iKey, bool * oOk = nullptr) const     { return static_cast<qint32>(value(iKey).toInt(oOk)); }
    quint32                 toUInt32(const QString iKey, bool * oOk = nullptr) const    { return static_cast<quint32>(value(iKey).toUInt(oOk)); }
    qint64                  toInt64(const QString iKey, bool * oOk = nullptr) const     { return static_cast<qint64>(value(iKey).toLongLong(oOk)); }
    quint64                 toUInt64(const QString iKey, bool * oOk = nullptr) const    { return static_cast<quint64>(value(iKey).toULongLong(oOk)); }
    QByteArray              toByteArray(const QString iKey) const                       { return QByteArray::fromBase64(value(iKey).toByteArray()); }
    QString                 toString(const QString iKey) const                          { return value(iKey).toString(); }
    QStringList             toStringList(const QString iKey) const                      { return value(iKey).toStringList(); }
    QVariantList            toVariantList(const QString iKey) const                     { return value(iKey).toList(); }
    QVariantMap             toVariantMap(const QString iKey) const                      { return value(iKey).toMap(); }

protected:
    void _copy(const Config &iOther);

    mutable QReadWriteLock  mLock;
    bool                    mDisableSave            = false;
    QStringList             mDefaultKeyFilter;
    QString                 mFilename;
    QVariantMap             mConfigValues;
    QMap< QString, void * > mPointers;
};

#endif // CONFIG_H
