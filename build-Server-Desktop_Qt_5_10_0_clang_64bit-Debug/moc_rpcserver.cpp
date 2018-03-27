/****************************************************************************
** Meta object code from reading C++ file 'rpcserver.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../RPC/rpcserver.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'rpcserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RPCServer_t {
    QByteArrayData data[15];
    char stringdata0[209];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RPCServer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RPCServer_t qt_meta_stringdata_RPCServer = {
    {
QT_MOC_LITERAL(0, 0, 9), // "RPCServer"
QT_MOC_LITERAL(1, 10, 22), // "failedToStartListening"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "startedListening"
QT_MOC_LITERAL(4, 51, 21), // "failedToStopListening"
QT_MOC_LITERAL(5, 73, 16), // "stoppedListening"
QT_MOC_LITERAL(6, 90, 13), // "newConnection"
QT_MOC_LITERAL(7, 104, 14), // "startListening"
QT_MOC_LITERAL(8, 119, 5), // "iPort"
QT_MOC_LITERAL(9, 125, 11), // "iServerName"
QT_MOC_LITERAL(10, 137, 17), // "QSslConfiguration"
QT_MOC_LITERAL(11, 155, 17), // "iSslConfiguration"
QT_MOC_LITERAL(12, 173, 12), // "QHostAddress"
QT_MOC_LITERAL(13, 186, 8), // "iAddress"
QT_MOC_LITERAL(14, 195, 13) // "stopListening"

    },
    "RPCServer\0failedToStartListening\0\0"
    "startedListening\0failedToStopListening\0"
    "stoppedListening\0newConnection\0"
    "startListening\0iPort\0iServerName\0"
    "QSslConfiguration\0iSslConfiguration\0"
    "QHostAddress\0iAddress\0stopListening"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RPCServer[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,
       5,    0,   62,    2, 0x06 /* Public */,
       6,    0,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    4,   64,    2, 0x0a /* Public */,
       7,    3,   73,    2, 0x2a /* Public | MethodCloned */,
       7,    4,   80,    2, 0x0a /* Public */,
      14,    0,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::UShort, QMetaType::QString, 0x80000000 | 10, 0x80000000 | 12,    8,    9,   11,   13,
    QMetaType::Void, QMetaType::UShort, QMetaType::QString, 0x80000000 | 10,    8,    9,   11,
    QMetaType::Void, 0x80000000 | 12, QMetaType::UShort, QMetaType::QString, 0x80000000 | 10,   13,    8,    9,   11,
    QMetaType::Void,

       0        // eod
};

void RPCServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        RPCServer *_t = static_cast<RPCServer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->failedToStartListening(); break;
        case 1: _t->startedListening(); break;
        case 2: _t->failedToStopListening(); break;
        case 3: _t->stoppedListening(); break;
        case 4: _t->newConnection(); break;
        case 5: _t->startListening((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QSslConfiguration(*)>(_a[3])),(*reinterpret_cast< const QHostAddress(*)>(_a[4]))); break;
        case 6: _t->startListening((*reinterpret_cast< quint16(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QSslConfiguration(*)>(_a[3]))); break;
        case 7: _t->startListening((*reinterpret_cast< const QHostAddress(*)>(_a[1])),(*reinterpret_cast< quint16(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QSslConfiguration(*)>(_a[4]))); break;
        case 8: _t->stopListening(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSslConfiguration >(); break;
            }
            break;
        case 6:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 2:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSslConfiguration >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 3:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QSslConfiguration >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (RPCServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCServer::failedToStartListening)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (RPCServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCServer::startedListening)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (RPCServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCServer::failedToStopListening)) {
                *result = 2;
                return;
            }
        }
        {
            typedef void (RPCServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCServer::stoppedListening)) {
                *result = 3;
                return;
            }
        }
        {
            typedef void (RPCServer::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&RPCServer::newConnection)) {
                *result = 4;
                return;
            }
        }
    }
}

const QMetaObject RPCServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_RPCServer.data,
      qt_meta_data_RPCServer,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *RPCServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RPCServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RPCServer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RPCServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void RPCServer::failedToStartListening()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RPCServer::startedListening()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RPCServer::failedToStopListening()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RPCServer::stoppedListening()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RPCServer::newConnection()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
