/****************************************************************************
** Meta object code from reading C++ file 'walletapp.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Mobile/walletapp.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'walletapp.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WalletApp_t {
    QByteArrayData data[12];
    char stringdata0[111];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WalletApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WalletApp_t qt_meta_stringdata_WalletApp = {
    {
QT_MOC_LITERAL(0, 0, 9), // "WalletApp"
QT_MOC_LITERAL(1, 10, 12), // "titleChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 12), // "countChanged"
QT_MOC_LITERAL(4, 37, 14), // "walletsChanged"
QT_MOC_LITERAL(5, 52, 8), // "listener"
QT_MOC_LITERAL(6, 61, 6), // "param1"
QT_MOC_LITERAL(7, 68, 6), // "param2"
QT_MOC_LITERAL(8, 75, 5), // "title"
QT_MOC_LITERAL(9, 81, 5), // "count"
QT_MOC_LITERAL(10, 87, 7), // "wallets"
QT_MOC_LITERAL(11, 95, 15) // "QList<QObject*>"

    },
    "WalletApp\0titleChanged\0\0countChanged\0"
    "walletsChanged\0listener\0param1\0param2\0"
    "title\0count\0wallets\0QList<QObject*>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WalletApp[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       3,   42, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,
       4,    0,   36,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    2,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    6,    7,

 // properties: name, type, flags
       8, QMetaType::QString, 0x00495103,
       9, QMetaType::Int, 0x00495001,
      10, 0x80000000 | 11, 0x00495009,

 // properties: notify_signal_id
       0,
       1,
       2,

       0        // eod
};

void WalletApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        WalletApp *_t = static_cast<WalletApp *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->titleChanged(); break;
        case 1: _t->countChanged(); break;
        case 2: _t->walletsChanged(); break;
        case 3: _t->listener((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (WalletApp::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletApp::titleChanged)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (WalletApp::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletApp::countChanged)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (WalletApp::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&WalletApp::walletsChanged)) {
                *result = 2;
                return;
            }
        }
    } else if (_c == QMetaObject::RegisterPropertyMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QObject*> >(); break;
        }
    }

#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        WalletApp *_t = static_cast<WalletApp *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->getTitle(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->getCount(); break;
        case 2: *reinterpret_cast< QList<QObject*>*>(_v) = _t->listWallets(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        WalletApp *_t = static_cast<WalletApp *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTitle(*reinterpret_cast< QString*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject WalletApp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_WalletApp.data,
      qt_meta_data_WalletApp,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *WalletApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WalletApp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WalletApp.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int WalletApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void WalletApp::titleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void WalletApp::countChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void WalletApp::walletsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
