/****************************************************************************
** Meta object code from reading C++ file 'wallet.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Core/wallet.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'wallet.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Wallet_t {
    QByteArrayData data[12];
    char stringdata0[103];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Wallet_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Wallet_t qt_meta_stringdata_Wallet = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Wallet"
QT_MOC_LITERAL(1, 7, 13), // "amountChanged"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 12), // "countChanged"
QT_MOC_LITERAL(4, 35, 12), // "billsChanged"
QT_MOC_LITERAL(5, 48, 4), // "type"
QT_MOC_LITERAL(6, 53, 8), // "currency"
QT_MOC_LITERAL(7, 62, 5), // "owner"
QT_MOC_LITERAL(8, 68, 6), // "amount"
QT_MOC_LITERAL(9, 75, 5), // "count"
QT_MOC_LITERAL(10, 81, 5), // "bills"
QT_MOC_LITERAL(11, 87, 15) // "QList<QObject*>"

    },
    "Wallet\0amountChanged\0\0countChanged\0"
    "billsChanged\0type\0currency\0owner\0"
    "amount\0count\0bills\0QList<QObject*>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Wallet[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       6,   32, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x06 /* Public */,
       3,    0,   30,    2, 0x06 /* Public */,
       4,    0,   31,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       5, QMetaType::Int, 0x00095001,
       6, QMetaType::QString, 0x00095001,
       7, QMetaType::QString, 0x00095001,
       8, QMetaType::QString, 0x00495001,
       9, QMetaType::Int, 0x00495001,
      10, 0x80000000 | 11, 0x00495009,

 // properties: notify_signal_id
       0,
       0,
       0,
       0,
       1,
       2,

       0        // eod
};

void Wallet::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Wallet *_t = static_cast<Wallet *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->amountChanged(); break;
        case 1: _t->countChanged(); break;
        case 2: _t->billsChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (Wallet::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Wallet::amountChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (Wallet::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Wallet::countChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (Wallet::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Wallet::billsChanged)) {
                *result = 2;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QObject*> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        Wallet *_t = static_cast<Wallet *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->getType(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->getCurrency(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->getOwner(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->getAmount(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->getCount(); break;
        case 5: *reinterpret_cast< QList<QObject*>*>(_v) = _t->listBills(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject Wallet::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Wallet.data,
      qt_meta_data_Wallet,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Wallet::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Wallet::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Wallet.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Wallet::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Wallet::amountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Wallet::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Wallet::billsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
