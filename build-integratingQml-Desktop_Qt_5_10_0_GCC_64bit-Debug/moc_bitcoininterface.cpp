/****************************************************************************
** Meta object code from reading C++ file 'bitcoininterface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Core/bitcoininterface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bitcoininterface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BitcoinInterface_t {
    QByteArrayData data[8];
    char stringdata0[97];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BitcoinInterface_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BitcoinInterface_t qt_meta_stringdata_BitcoinInterface = {
    {
QT_MOC_LITERAL(0, 0, 16), // "BitcoinInterface"
QT_MOC_LITERAL(1, 17, 13), // "walletCreated"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 16), // "BitcoinWalletRef"
QT_MOC_LITERAL(4, 49, 7), // "iWallet"
QT_MOC_LITERAL(5, 57, 20), // "walletFailedToCreate"
QT_MOC_LITERAL(6, 78, 12), // "createWallet"
QT_MOC_LITERAL(7, 91, 5) // "iTest"

    },
    "BitcoinInterface\0walletCreated\0\0"
    "BitcoinWalletRef\0iWallet\0walletFailedToCreate\0"
    "createWallet\0iTest"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BitcoinInterface[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    0,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    1,   38,    2, 0x0a /* Public */,
       6,    0,   41,    2, 0x2a /* Public | MethodCloned */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    7,
    QMetaType::Void,

       0        // eod
};

void BitcoinInterface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BitcoinInterface *_t = static_cast<BitcoinInterface *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->walletCreated((*reinterpret_cast< BitcoinWalletRef(*)>(_a[1]))); break;
        case 1: _t->walletFailedToCreate(); break;
        case 2: _t->createWallet((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->createWallet(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< BitcoinWalletRef >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (BitcoinInterface::*_t)(BitcoinWalletRef );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitcoinInterface::walletCreated)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (BitcoinInterface::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BitcoinInterface::walletFailedToCreate)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject BitcoinInterface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BitcoinInterface.data,
      qt_meta_data_BitcoinInterface,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BitcoinInterface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BitcoinInterface::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BitcoinInterface.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int BitcoinInterface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void BitcoinInterface::walletCreated(BitcoinWalletRef _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void BitcoinInterface::walletFailedToCreate()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
